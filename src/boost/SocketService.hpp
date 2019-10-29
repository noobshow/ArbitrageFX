#ifndef SOCKETSERVICE_BOOST_H
#define SOCKETSERVICE_BOOST_H

#include <iostream>

#include "Dispatcher.h"
#include "Service.h"

using boost::asio::ip::tcp;
using namespace std;

class SocketService :
        public Service,
        public Dispatcher<proto::RawCommand>
{
private:
    std::shared_ptr<tcp::socket> socketPtr;
    tcp::endpoint endpoint;
    tcp::acceptor acceptor;
    tcp::resolver resolver;
    int port;
    std::shared_ptr<SocketService> servicePtr;
public:
    SocketService(int _port) ;
    ~SocketService() { this->io_service.stop(); }
    //tcp::socket& getSocket() { return this->socket; }
    void resetConnection();
    void read(const boost::system::error_code& error);
    void write(std::string msg);
    void handle_read(const boost::system::error_code& error);

    void setServicePtr(std::shared_ptr<SocketService> ptr) { this->servicePtr = ptr; }
    std::shared_ptr<SocketService> getServicePtr() { return this->servicePtr; }
    int getPort() { return this->port; }
};

typedef std::shared_ptr<SocketService> SocketServicePtr;


#endif // SOCKETSERVICE_BOOST_H
