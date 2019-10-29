#ifndef SERVICE_H
#define SERVICE_H

#include <boost/utility.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

enum { max_length = 100000 };

class Service
{
protected:
    char buffer[max_length];
    boost::asio::io_service io_service;

public:
    bool active;

    Service() :
        io_service(),
        active(false)
    { }

    virtual void read(const boost::system::error_code& error) = 0;
    virtual void write(std::string msg) = 0;
};

typedef std::shared_ptr<Service> ServicePtr;

#endif // SERVICE_H
