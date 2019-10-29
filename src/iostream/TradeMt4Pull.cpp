#include "TradeMt4Pull.hpp"

TradeMt4Pull::TradeMt4Pull(std::string bind) :
    running_(false),
    bind_(bind)
{
}

TradeMt4Pull::~TradeMt4Pull()
{
    this->stop();
}

void TradeMt4Pull::stop()
{
    running_ = false;
    if(threadPullServer.joinable()){
        threadPullServer.join();
    }
}

void TradeMt4Pull::start()
{
    running_ = true;
    threadPullServer = std::thread( &TradeMt4Pull::processPullServer, this );
}

void TradeMt4Pull::processPullServer()
{
    LOG_WARNING << "Iniciando Servidor TRADE PULL -> " << bind_;
    zmq::context_t context(1);
    zmq::socket_t receiver(context,ZMQ_PULL);
    receiver.bind(bind_);

    LOG_WARNING << "Recebendo mensagens no servidor TRADE PULL " << bind_;
    while(running_)
    {
        zmq::message_t message;
        receiver.recv(&message);

        std::string msg(static_cast<char*>(message.data()), message.size());
        //LOG_INFO << "PUSH Message received, size: " << message.size() << ", msg: " << msg;
        Notify(std::make_shared<std::string>(msg));

    }
}
