#ifndef SERVICE_H
#define SERVICE_H

#include <string>
#include <memory>

#include "common/Dispatcher.hpp"

enum { max_length = 100000 };

class Service;

typedef std::shared_ptr<Service> ServicePtr;
typedef std::pair<std::string, ServicePtr> RawCommand;
typedef std::shared_ptr<RawCommand> RawCommandPtr;

class Service : public Dispatcher<RawCommand>
{
protected:
    char buffer[max_length];
    bool active;
    int port;
    std::shared_ptr<Service> servicePtr;
public:
    Service(int _port) : active(false), port(_port) { }
    virtual ~Service() = default;
    int getPort() { return this->port; }
    void setServicePtr(std::shared_ptr<Service> ptr) { this->servicePtr = ptr; }
    std::shared_ptr<Service> getServicePtr() { return this->servicePtr; }

    virtual void read() = 0;
    virtual void write(std::string msg) = 0;

};

#endif // SERVICE_H
