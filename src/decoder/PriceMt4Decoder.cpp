#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include "PriceMt4Decoder.hpp"

PriceMt4Decoder::PriceMt4Decoder() : PoolObserver(), realtime_(true)
{
}


void PriceMt4Decoder::AsyncUpdate(std::shared_ptr<std::string> cmdStr)
{
    std::vector<std::string> tokens;
    boost::split(tokens, *cmdStr, boost::is_any_of(DELIMITER));

    if(tokens.size() > 1) // 1,16038,FFS,AUDJPY,81553,81575
    {
        int id = atoi(tokens[0].c_str());
        bool invCmd = false;

        switch (id)
        {
        case CMD_PRICE:
            if(tokens.size() == 6){
                TickPtr tickPtr =
                        std::make_shared<tick_s>(
                            tokens[1], tokens[2], tokens[3], tokens[4], tokens[5]);
                if(tickPtr->valid_)
                {
                    std::map<std::string, std::chrono::milliseconds>::iterator it;
                    it = this->brokerStartTime.find(tickPtr->broker_);

                    if(it == this->brokerStartTime.end())
                    {
                        uint64_t broker_timestamp = tickPtr->timestamp_;
                        std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::system_clock::now().time_since_epoch());

                        tickPtr->timestamp_ = uint64_t(ms.count());

                        ms -= std::chrono::milliseconds(broker_timestamp);
                        this->brokerStartTime.insert(std::pair<std::string, std::chrono::milliseconds>(tickPtr->broker_, ms));
                    }
                    else{
                        std::chrono::milliseconds ms = it->second;
                        ms += std::chrono::milliseconds(tickPtr->timestamp_);
                        tickPtr->timestamp_ = uint64_t(ms.count());
                    }

                    Notify(tickPtr);
                }
                else invCmd = true;
            }
            else invCmd = true;
            break;
        default:
            LOG_ERROR << "ERROR Invalid ID: " << cmdStr;
            break;
        }
        if(invCmd){
            LOG_ERROR << "ERROR Invalid CMD " << id << ": " << cmdStr;
        }
    }
    else{
        LOG_ERROR << "ERROR Invalid tokens parse: " << cmdStr;
    }


}
