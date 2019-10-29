#include "fix/FixServer.hpp"
#include "common/loguru.hpp"

FixServer::FixServer(string _configFile, bool _useSSL)
    : configFile(_configFile) ,
      useSSL(_useSSL)
{
}

bool FixServer::start()
{
    try
    {
        //this->settings = std::make_unique<FIX::SessionSettings>(this->configFile);
        //this->storeFactory = std::make_unique<FIX::FileStoreFactory>(*this->settings);
        //this->logFactory = std::make_unique<FIX::ScreenLogFactory>(*this->settings);

        this->settings.reset(new FIX::SessionSettings(this->configFile));
        this->storeFactory.reset(new FIX::FileStoreFactory(*this->settings));
        this->logFactory.reset(new FIX::ScreenLogFactory(*this->settings));

        unique_ptr<FIX::Log> log;

        std::set<FIX::SessionID> sessions = this->settings->getSessions();

        for(FIX::SessionID session : sessions)
        {
            LOG_INFO << "Sessions -> " << session.toString();
            log.reset(this->logFactory->create(session));
            std::cout  << session.getBeginString() << std::endl;
            std::cout  << session.getSenderCompID() << std::endl;
            std::cout  << session.getTargetCompID()<< std::endl;
        }

        FIX::Dictionary dic = this->settings->get();

        FIX::Dictionary::Data::const_iterator it;
        //map<string, string>::iterator it;
        for ( it = dic.begin(); it != dic.end(); it++ )
        {
            LOG_INFO << "Dictionary Map -> " << "Key: " << it->first << ", Value: " << it->second;
        }

        this->application.reset( new FixApplication(
                    sessions.begin()->getBeginString(),
                    sessions.begin()->getSenderCompID(),
                    sessions.begin()->getTargetCompID()
                    ));

//        if(this->useSSL){
//          this->initiator.reset(new FIX::SSLSocketInitiator( // FIX::SSLSocketInitiator/ThreadedSSLSocketInitiator
//                        *application, *storeFactory, *settings, *logFactory ));

//        }
//        else{
            this->initiator.reset(new FIX::SocketInitiator( // FIX::SSLSocketInitiator
                          *application, *storeFactory, *settings, *logFactory ));
//        }
        this->initiator->start();



    }
    catch ( std::exception & e )
    {
        LOG_ERROR << "Exception FixServer::start:" << e.what();
        throw "Exception FixServer::start";
        return false;
    }
    return true;

}

void FixServer::stop(){


}
