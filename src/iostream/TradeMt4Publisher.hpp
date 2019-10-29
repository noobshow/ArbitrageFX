#ifndef TRADEMT4PUBLISHER_H
#define TRADEMT4PUBLISHER_H

#include <zmq.hpp>

#include "common/Types.hpp"
#include "iostream/Mt4Publisher.hpp"

class TradeMt4Publisher : public Mt4Publisher
{
private:
    zmq::context_t context;
    zmq::socket_t publisher;
public:
    TradeMt4Publisher(std::string bind);

    void AsyncUpdate(RequestCommandPtr reqCmdPtr);
};

typedef std::shared_ptr<TradeMt4Publisher> TradeMt4PublisherPtr;

#endif // TRADEMT4PUBLISHER_H
