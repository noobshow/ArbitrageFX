#include "common/SocketControl.hpp"
#include "qt/loguru.hpp"
#include "qt/SocketService.hpp"


SocketControl::SocketControl(){
}

ServicePtr SocketControl::getSocketPtr(int port)
{
    ServicePtr socketServicePtr = nullptr;
    for (ServicePtr aux : this->serviceList) {
        if(aux->getPort() == port){
            socketServicePtr = aux;
            break;
        }
    }
    return socketServicePtr;
}

bool SocketControl::startServers(int port_begin, int port_end)
{
    try{
        this->portBegin = port_begin;
        this->portEnd = port_end;
        int totalPorts = portEnd - portBegin;
        LOG_INFO << "Iniciando servidores tcp...";
        if(totalPorts <= 0 || totalPorts > 50){
            LOG_ERROR << "SocketControl::startServers: Portas invalidas ("
                      << portBegin << "-" << portEnd << ")";
            return false;
        }

        for (int port = portBegin; port <= portEnd; port++)
        {
            LOG_INFO << "Criando servidor tcp na porta: " << port << "...";
            ServicePtr servicePtr = make_shared<SocketService>(port);
            servicePtr->setServicePtr(servicePtr);
            serviceList.push_back(servicePtr);

            std::shared_ptr<thread> socketServer = make_shared<thread>(std::thread(&Service::read, servicePtr));
            threadServiceList.push_back(socketServer);
        }
    }
    catch (std::exception & e)
    {
        LOG_ERROR << "Exception in SocketControl::startServers: " << e.what();
        return false;
    }
    return true;
}

void SocketControl::attachObserverRawCommand(std::shared_ptr<Observer<RawCommand> > observerPtr)
{
    for(ServicePtr servicePtr : serviceList){
        servicePtr->Attach(observerPtr);
    }
}
