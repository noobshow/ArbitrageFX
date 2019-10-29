#ifndef PriceMt4Pull_H
#define PriceMt4Pull_H

#include <QObject>
#include <sstream>
#include <zmq.hpp>
#include <thread>

#include "iostream/IoStream.hpp"
#include "common/mt4Protocol.hpp"
#include "common/loguru.hpp"

class PriceMt4Pull : public PriceMt4Stream
{
public:
    PriceMt4Pull(std::string bind);
    ~PriceMt4Pull();
    void start();
    void stop();
private:
    std::thread threadPullServer;
    void processPullServer();
    bool running_;
    std::string bind_;
};

#endif // PriceMt4Pull_H
