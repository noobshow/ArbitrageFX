#ifndef PRICEMT4DECODER_H
#define PRICEMT4DECODER_H

#include <memory>
#include <vector>
#include <chrono>
#include <map>

#include "common/loguru.hpp"
#include "common/Dispatcher.hpp"
#include "common/Observer.hpp"
#include "common/Types.hpp"

class PriceMt4Decoder:
        public Dispatcher<tick_s>,
        public PoolObserver<std::string>
{
private:
    bool realtime_;
public:
    PriceMt4Decoder();
    void AsyncUpdate(std::shared_ptr<std::string> cmdStr);
    std::map<std::string, std::chrono::milliseconds> brokerStartTime;
    void setRealTime(bool realTime){
        this->realtime_ = realTime;
    }
};

typedef std::shared_ptr<PriceMt4Decoder> PriceMt4DecoderPtr;

#endif // PRICEMT4DECODER_H


