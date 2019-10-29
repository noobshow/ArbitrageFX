#ifndef TradeMt4Pull_H
#define TradeMt4Pull_H

#include <QObject>
#include <sstream>
#include <zmq.hpp>
#include <thread>

#include "iostream/IoStream.hpp"
#include "common/loguru.hpp"

class TradeMt4Pull : public TradeMt4Stream
{
public:
    TradeMt4Pull(std::string bind);
    ~TradeMt4Pull();
    void start();
    void stop();
private:
    std::thread threadPullServer;
    void processPullServer();
    bool running_;
    std::string bind_;
};

#endif // TradeMt4Pull_H
