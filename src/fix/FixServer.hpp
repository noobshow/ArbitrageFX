#ifndef FIXSERVER_H
#define FIXSERVER_H

#include <memory>
#include <iostream>
#include <quickfix/FileStore.h>
#include <quickfix/FileLog.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/ThreadedSSLSocketInitiator.h>
#include <quickfix/SSLSocketInitiator.h>
#include "fix/FixApplication.hpp"

using namespace std;

class FixServer
{
private:
    unique_ptr<FixApplication> application;
    unique_ptr<FIX::Initiator> initiator;
    unique_ptr<FIX::SessionSettings> settings;
    unique_ptr<FIX::FileStoreFactory> storeFactory;
    unique_ptr<FIX::ScreenLogFactory> logFactory;
    string configFile;
    bool useSSL;

public:
    FixServer(string _configFile, bool _useSSL);
    bool start();
    void stop();
};
#endif // FIXSERVER_H
