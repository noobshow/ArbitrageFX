#ifndef TRADEMT4DECODER_H
#define TRADEMT4DECODER_H

#include <memory>
#include <vector>
#include <chrono>
#include <map>

#include "common/loguru.hpp"
#include "common/Dispatcher.hpp"
#include "common/AsyncThreadProcess.hpp"
#include "common/Types.hpp"

class TradeMt4Decoder:
        public Dispatcher<order_resp_cmd_s>,
        public AsyncObserver<std::string>
{
private:
public:
    TradeMt4Decoder();
    void AsyncUpdate(std::shared_ptr<std::string> cmdStr);
    std::map<std::string, std::chrono::milliseconds> brokerStartTime;
};

typedef std::shared_ptr<TradeMt4Decoder> TradeMt4DecoderPtr;

#endif // TRADEMT4DECODER_H


