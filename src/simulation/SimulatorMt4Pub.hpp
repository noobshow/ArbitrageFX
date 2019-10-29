#ifndef SIMULATORMT4PUB_H
#define SIMULATORMT4PUB_H

#include "iostream/Mt4Publisher.hpp"
#include "iostream/IoStream.hpp"
#include "common/Types.hpp"

class SimulatorMt4Pub : public Mt4Publisher, public TradeMt4Stream
{
public:
    void AsyncUpdate(RequestCommandPtr reqCmdPtr);
    void start() {}
    void stop() {}
};

typedef std::shared_ptr<SimulatorMt4Pub> SimulatorMt4PubPtr;
#endif // SIMULATORMT4PUB_H
