#ifndef SOCKETCONTROL_H
#define SOCKETCONTROL_H

#include <memory>
#include <vector>
#include <thread>

#include "common/Service.hpp"
#include "mt4/mt4Protocol.hpp"
#include "common/Singleton.hpp"

class SocketControl : public Singleton<SocketControl>
{
private:
    std::vector<ServicePtr> serviceList;
    std::vector<std::shared_ptr<std::thread>> threadServiceList;
    int portBegin, portEnd;

    void processCommands();

public:
    SocketControl();
    ~SocketControl() { }
    bool startServers(int port_begin, int port_end);
    ServicePtr getSocketPtr(int port);
    void attachObserverRawCommand(std::shared_ptr<Observer<RawCommand>> observerPtr);
};


typedef std::shared_ptr<SocketControl> SocketControlPtr;

#endif // SOCKETCONTROL_H
