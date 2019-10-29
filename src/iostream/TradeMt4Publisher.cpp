#include "TradeMt4Publisher.hpp"
#include "common/zhelpers.hpp"

TradeMt4Publisher::TradeMt4Publisher(std::string bind) :
    context(1),
    publisher(context, ZMQ_PUB)
{
    this->publisher.bind(bind);
}

void TradeMt4Publisher::AsyncUpdate(RequestCommandPtr reqCmdPtr)
{
    LOG_INFO << "MT4 Publisher RequestCommand: " << reqCmdPtr->to_cmd_string();

    s_sendmore (publisher, reqCmdPtr->broker_);
    s_send(publisher, reqCmdPtr->to_cmd_string());
}

