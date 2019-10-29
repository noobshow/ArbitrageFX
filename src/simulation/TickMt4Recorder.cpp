#include "TickMt4Recorder.hpp"
#include "common/Utils.hpp"

TickMt4Recorder::TickMt4Recorder(TickMt4Format tickMt4Format)
{
    this->format = tickMt4Format;

    if(this->format.path.empty()){
        this->format.path = "TickMt4Recorder/";
    }
    else{
        if(this->format.path.back() != '/')
            this->format.path += "/";
    }

    fs::create_directories(this->format.path);
    if(!fs::exists(this->format.path))
        this->format.path.clear();

    this->time = std::time(nullptr);
    this->tmPtr = std::localtime(&this->time);

    this->lastWriteMinute = this->tmPtr->tm_min;
    this->lastFileMinute = this->tmPtr->tm_min;
    this->lastFileHour = this->tmPtr->tm_hour;
    if(this->format.realTime)
    {
        this->threadProcess = std::thread(&TickMt4Recorder::process, this);
        this->threadProcess.detach();
    }
}

TickMt4Recorder::~TickMt4Recorder(){
    if(this->file.is_open()) this->file.close();
}

void TickMt4Recorder::AsyncUpdate(TickPtr tickPtr){
    this->mutex.lock();
    this->buffer_.push_back(tickPtr);
    this->mutex.unlock();

    if(!this->format.realTime)
    {
        auto in_time_t = TimestampToTime_t(tickPtr->timestamp_);
        this->tmPtr = std::localtime(&in_time_t);
        this->checkFileRecorder();
    }

}

void TickMt4Recorder::process(){

    try{
        while(true)
        {
            if(this->buffer_.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            this->time = std::time(nullptr);
            this->tmPtr = std::localtime(&this->time);

            this->checkFileRecorder();

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    catch (...)
    {
        LOG_ERROR << "TickMt4Recorder::process: Erro na gravação do arquivo. ";
    }

}

void TickMt4Recorder::checkFileRecorder()
{
    if(this->lastWriteMinute != this->tmPtr->tm_min &&
      (this->tmPtr->tm_min % this->format.minuteToWrite) == 0)
    {
        if(this->format.binary){
            this->file.open(this->filename.str(), std::ofstream::out | std::ofstream::app | std::ofstream::binary);
        }
        else{
            this->file.open(this->filename.str(), std::ofstream::out | std::ofstream::app);
        }

        this->mutex.lock();

        for( auto tickPtr : this->buffer_)
        {
            if(this->format.brokers.size() != 0){
                if(this->format.brokers.find(tickPtr->broker_) == this->format.brokers.end()){
                    continue;
                }
            }
            if(this->format.symbols.size() != 0){
                if(this->format.symbols.find(tickPtr->symbol_) == this->format.symbols.end()){
                    continue;
                }
            }

            if(this->format.binary){
                 this->file.write( (char*)tickPtr.get(), sizeof(*tickPtr));
            }
            else{
                if(this->format.field_timeStamp){
                    if(this->format.datetime)
                    {
                        //auto in_time_t = TimestampToTime_t(tickPtr->timestamp_);
                        //this->file << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%X");
                        this->file << TimestampToFormattedString(tickPtr->timestamp_);
                    }
                    else{
                        this->file << tickPtr->timestamp_;
                    }
                    this->file << this->format.delimiter;
                }
                if(this->format.field_broker){
                    this->file << tickPtr->broker_ << this->format.delimiter;
                }
                if(this->format.field_symbol){
                    this->file << tickPtr->symbol_ << this->format.delimiter;
                }
                if(this->format.field_ask){
                    if(this->format.priceCommissionFix)
                        //this->PriceCommissionFix(tickPtr->broker_, tickPtr->ask_, tickPtr->bid_);

                    this->file << tickPtr->ask_ << this->format.delimiter;
                }
                if(this->format.field_bid){
                    this->file << tickPtr->bid_ << this->format.delimiter;
                }
                if(this->format.field_spread){
                    this->file << (tickPtr->ask_ - tickPtr->bid_) << this->format.delimiter;
                }
                if(this->format.field_spreadPercent){
                    double spreadPercent = double(tickPtr->ask_ - tickPtr->bid_) / double(tickPtr->bid_);
                    this->file << std::fixed << std::setprecision(6) << spreadPercent << this->format.delimiter;
                }
                if(this->format.field_priceDiff){
                    int32_t diff = 99;

                    std::map<std::string, std::pair<int32_t, int32_t>>::iterator it;
                    bid_ask bid_ask_1 = bid_ask(tickPtr->bid_, tickPtr->ask_);

                    it = this->lastBrokerPrice.find(tickPtr->broker_);
                    if(it == this->lastBrokerPrice.end()){
                        this->lastBrokerPrice.insert(
                                    std::pair<std::string, bid_ask>( tickPtr->broker_, bid_ask_1));
                    }
                    else{
                        for( auto& it_pair : this->lastBrokerPrice )
                        {
                            if(it_pair.first == tickPtr->broker_){
                                it_pair.second = bid_ask_1;
                            }
                            else{

                                bid_ask bid_ask_2 = it_pair.second;
                                int32_t diff1 = bid_ask_1.second - bid_ask_2.first;
                                int32_t diff2 = bid_ask_2.second - bid_ask_1.first;
                                if(diff1 < diff) diff = diff1;
                                if(diff2 < diff) diff = diff2;
                            }

                        }
                    }
                    this->file << diff << this->format.delimiter;
                }
                this->file << std::endl;
            }

        }
        this->buffer.clear();
        this->mutex.unlock();
        this->file.close();
        this->lastWriteMinute = this->tmPtr->tm_min;
    }


    if(this->lastFileMinute != this->tmPtr->tm_min &&
      (this->tmPtr->tm_min % this->format.minutesPerFile) == 0)
    {
        if(this->file.is_open()) this->file.close();

        std::string file_saved = this->filename.str();
        this->filename = std::stringstream();
        this->filename << this->format.path << "TickMt4Rec_" << this->tmPtr->tm_year+1900 << "_"
                       << std::setw(2) << std::setfill('0') << this->tmPtr->tm_mon+1 << "_"
                       << std::setw(2) << std::setfill('0') << this->tmPtr->tm_mday << "-"
                       << std::setw(2) << std::setfill('0') << this->lastFileHour << "-"
                       << std::setw(2) << std::setfill('0') << this->lastFileMinute << ".bin";

        LOG_INFO << "Finalizando gravação de arquivo " << file_saved;
        LOG_INFO << "Iniciando gravação de arquivo " << this->filename.str();
//        LOG_INFO << ". lastMin: " << this->lastFileMinute << ", tm_min: " << this->tmPtr->tm_min
//                 << ", minPerFile: " << this->format.minutesPerFile;


        this->lastFileMinute = this->tmPtr->tm_min;
        this->lastFileHour  = this->tmPtr->tm_hour;
    }
}

//void TickMt4Recorder::PriceCommissionFix(std::string broker, int &ask, int &bid)
//{
//    if(broker == "ICM"){
//        ask += 7;
//        bid -= 7;
//    }
//}


