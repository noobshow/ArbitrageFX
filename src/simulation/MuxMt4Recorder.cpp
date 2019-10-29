#include "MuxMt4Recorder.hpp"

MuxMt4Recorder::MuxMt4Recorder(string path, int minutesPerFile)
{
    if(path.empty()){
        this->path = "MuxMt4Recorder/";
    }
    else{
        if(path.back() != '/') path += "/";
        this->path = path;
    }

    //boost::filesystem::path dir(this->path.c_str());
    //boost::filesystem::create_directories(dir);
    //fs::path(this->path);
    fs::create_directories(this->path);
    if(!fs::exists(this->path))
        this->path.clear();

    this->time = std::time(nullptr);
    this->tmPtr = std::localtime(&this->time);

    this->timeInMinutes = minutesPerFile;
    this->lastMinute = this->tmPtr->tm_min;
    this->lastHour = this->tmPtr->tm_hour;
    this->threadProcess = std::thread(&MuxMt4Recorder::process, this);
    this->threadProcess.detach();
}

MuxMt4Recorder::~MuxMt4Recorder(){
    if(this->file.is_open()) this->file.close();
}

void MuxMt4Recorder::AsyncUpdate(std::shared_ptr<std::string> stringPtr){
    this->mutex.lock();
    this->cmd_buffer.push_back(*stringPtr);
    this->mutex.unlock();
}

void MuxMt4Recorder::process(){

    try{
        while(true){
            this->time = std::time(nullptr);
            this->tmPtr = std::localtime(&this->time);


            if(this->lastMinute != this->tmPtr->tm_min &&
              (this->tmPtr->tm_min % this->timeInMinutes) == 0)
            {

                if(this->file.is_open()) this->file.close();
                stringstream ss;
                ss << this->path << "MuxMt4Cmd_" << this->tmPtr->tm_year+1900 << "_"
                   << std::setw(2) << std::setfill('0') << this->tmPtr->tm_mon+1 << "_"
                   << std::setw(2) << std::setfill('0') << this->tmPtr->tm_mday << "-"
                   << std::setw(2) << std::setfill('0') << this->lastHour << "-"
                   << std::setw(2) << std::setfill('0') << this->lastMinute << ".csv";

                cout << "Gravando arquivo " << ss.str() << ", lastMinute: " << this->lastMinute <<
                        ", tm_min: " << this->tmPtr->tm_min <<
                        ", timeInMinutes: " << this->timeInMinutes << endl;

                this->file.open(ss.str(), std::ofstream::out);

                this->mutex.lock();
                for( std::string stringCmd : this->cmd_buffer)
                    this->file << stringCmd.c_str() << std::endl;
                this->cmd_buffer.clear();
                this->mutex.unlock();

                this->file.close();
                this->lastMinute = this->tmPtr->tm_min;
                this->lastHour  = this->tmPtr->tm_hour;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    catch (...)
    {
        LOG_ERROR << "MuxMt4Recorder::process: Erro na gravação do arquivo. ";
    }

}

