#include <memory>

#include "SocketService.h"
#include "Log.h"

SocketService::SocketService(int _port)
    : Service(),
      socketPtr(std::make_shared<tcp::socket>(io_service)),
      endpoint(tcp::v4(), (unsigned short)_port),
      acceptor(io_service, endpoint),
      resolver(io_service),
      port(_port)
{
}

void SocketService::resetConnection()
{
    this->io_service.reset();
    this->socketPtr = std::make_shared<tcp::socket>(io_service);
}

void SocketService::handle_read(const boost::system::error_code& error)
{
    try
    {
        this->active = true;
        for (;;)
        {    
            boost::system::error_code error;
            boost::asio::streambuf response;
            boost::asio::read_until(*socketPtr, response, "\r\n");

            std::istream response_stream(&response);
            std::string line;
            std::getline(response_stream, line);

            Notify(std::make_shared<proto::RawCommand>(std::make_pair(line, this->getServicePtr())));

            //LOG_DEBUG << "SocketService::handle_read: new data, line: " << line ;

            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.


        }
    }
    catch (exception& e)
    {
        LOG_ERROR << "Exception em SocketService::handle_read: " << e.what();
    }
    this->active = false;
}

void SocketService::read(const boost::system::error_code& error)
{
    while(true){
        try
        {
            LOG_DEBUG << "Aguardando nova conexao TCP para leitura, porta(" << port << ")";
            this->acceptor.async_accept(*socketPtr, boost::bind(&SocketService::handle_read, this, boost::asio::placeholders::error));

            this->io_service.run();
            this->resetConnection();

        }
        catch (exception& e)
        {
            LOG_ERROR << "Exception em SocketService::read: " << e.what();
        }
    }
}

void SocketService::write(string msg)
{
    try
    {
        //tcp::resolver::query query(tcp::v4(), "localhost", std::to_string(port).c_str());
        //tcp::resolver::iterator it_endpoint = resolver.resolve(query);
        //LOG_DEBUG << "SocketService::write: Iniciando conexao TCP no endereco " << it_endpoint->endpoint();
        //this->socketPtr->connect(*it_endpoint);

        msg += "\r\n";
        LOG_DEBUG << "SocketService::write: enviando dados, bytes: " << msg.length()
             << ", mensagem: " << msg;

        this->socketPtr->write_some(boost::asio::buffer(msg.c_str(), msg.length()));

        LOG_DEBUG << "SocketService::write: Dados enviados com sucesso!";
    }
    catch (exception& e)
    {
        LOG_ERROR << "Exception em SocketService::write: " << e.what();
    }
}

