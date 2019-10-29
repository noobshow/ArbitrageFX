#include <iostream>
#include <chrono>
#include <thread>

#include "Config.h"
#include "SocketControl.h"
#include "ArbitrageCore.h"
#include "CommandDecoder.h"
#include "CommandDispatcher.h"
#include "Log.h"

#define FIXSERVER
#ifdef FIXSERVER
#include "FixServer.h"
#endif


using namespace std;

int main( int argc, char** argv )
{
    try
    {
        /* LOG EXAMPLES
         * LOG_TRACE << "this is a trace message";
         * LOG_DEBUG << "this is a debug message";
         * LOG_WARNING << "this is a warning message";
         * LOG_ERROR << "this is an error message";
         * LOG_FATAL << "this is a fatal error message";
         */

        //cout<<"Iniciando Arbitrage..."<<endl;
        LOG_DEBUG << "Iniciando Arbitrage...";

        Config::getInstance()->setFileName("arbitrage.ini");
        Config::getInstance()->load();

        int portBegin = Config::getInstance()->socket_port_begin;
        int portEnd = Config::getInstance()->socket_port_end;

        if(!SocketControl::getInstance()->startServers(portBegin, portEnd)){
            return EXIT_FAILURE;
        }

        CommandDecoderPtr commandDecodePtr = std::make_shared<CommandDecoder>();
        SocketControl::getInstance()->attachObserverRawCommand(commandDecodePtr);

        ArbitrageCorePtr arbitrageCorePtr = std::make_shared<ArbitrageCore>();
        commandDecodePtr->Attach(arbitrageCorePtr);

        CommandDispatcherPtr commandDispatcherPtr = std::make_shared<CommandDispatcher>();
        arbitrageCorePtr->Attach(commandDispatcherPtr);

        #ifdef FIXSERVER
        LOG_DEBUG << "Iniciando servidor FIX...";
        FixServer fixServer("FixConfigSqfin", false);
        fixServer.start();
        #endif // FIXSERVER

        while(true){
            sleep(1);
        }
    }
    catch ( std::exception & e )
    {
        LOG_ERROR << "Exception Main:" << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
