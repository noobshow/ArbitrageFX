#include "PriceMt4Pull.hpp"

PriceMt4Pull::PriceMt4Pull(std::string bind) :
    running_(false),
    bind_(bind)
{
}

PriceMt4Pull::~PriceMt4Pull()
{
    this->stop();
}

void PriceMt4Pull::stop()
{
    running_ = false;
    if(threadPullServer.joinable()){
        threadPullServer.join();
    }
}

void PriceMt4Pull::start()
{
    running_ = true;
    threadPullServer = std::thread( &PriceMt4Pull::processPullServer, this );
}

void PriceMt4Pull::processPullServer()
{
    LOG_WARNING << "Iniciando Servidor PRICE PULL -> " << bind_;
    zmq::context_t context(1);
    zmq::socket_t receiver(context,ZMQ_PULL);
    receiver.bind(bind_);

    LOG_WARNING << "Recebendo mensagens no servidor PRICE PULL " << bind_;
    while(running_)
    {
        zmq::message_t message;
        receiver.recv(&message);

        std::string msg(static_cast<char*>(message.data()), message.size());
        //LOG_INFO << "PUSH Message received, size: " << message.size() << ", msg: " << msg;
        Notify(std::make_shared<std::string>(msg));

    }
}
