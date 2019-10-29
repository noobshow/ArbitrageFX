#include <QCoreApplication>
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

#include <zmq.hpp>
#include <QThread>

#include "common/defines.hpp"
#include "common/ExecutionTimer.hpp"
#include "common/options.hpp"
#include "common/loguru.hpp"
#include "common/Config.hpp"

#include "arbitrage/ArbitrageManager.hpp"
#include "arbitrage/ArbitrageCore.hpp"

#include "decoder/PriceMt4Decoder.hpp"
#include "decoder/TradeMt4Decoder.hpp"

#include "iostream/PriceMt4Pull.hpp"
#include "iostream/TradeMt4Pull.hpp"
#include "iostream/TradeMt4Publisher.hpp"

#include "simulation/DemuxMt4Stream.hpp"
#include "simulation/MuxMt4Recorder.hpp"
#include "simulation/TickMt4Recorder.hpp"
#include "simulation/SimulatorMt4Pub.hpp"

#include "fix/FixServer.hpp"


int main( int argc, char** argv )
{
    QCoreApplication a(argc, argv);

    //loguru::add_file("arbitrage.log", loguru::Append, loguru::Verbosity_MAX);
    loguru::add_file("log/arbitrage.log", loguru::Append, loguru::Verbosity_INFO);
    //loguru::g_stderr_verbosity = 1; // Only show most relevant things on stderr:
    loguru::g_stderr_verbosity = loguru::Verbosity_MAX;
    LOG_WARNING << "Iniciando Arbitrage...";

    Config::getInstance()->fileName = "config/arbitrage.ini";
    Config::getInstance()->load();

    cxxopts::Options options(argv[0], " - Tiger arbitrage");
    options.allow_unrecognised_options().add_options()
            ("s,mt4stream", "MT4 Stream")
            ("d,demux", "Demux MT4 Stream <dir/file>", cxxopts::value<std::string>())
            ("m,mux", "Mux MT4 Recorder <dir/file>", cxxopts::value<std::string>())
            ("r,tickmt4recorder", "Tick MT4 Recorder <dir/file>", cxxopts::value<std::string>())
            ;
    auto result = options.parse(argc, argv);

    PriceMt4StreamPtr priceMt4StreamPtr;
    TradeMt4StreamPtr tradeMt4StreamPtr;
    MuxMt4RecorderPtr muxMt4RecorderPtr;
    DemuxMt4StreamPtr demuxMt4StreamPtr;
    TickMt4StreamPtr tickMt4StreamPtr;
    TickMt4RecorderPtr tickMt4RecorderPtr;
    Mt4PublisherPtr mt4PublisherPtr;
    ArbitrageManagerPtr arbitrageManagerPtr = std::make_shared<ArbitrageManager>();
    ArbitrageCorePtr arbitrageCorePtr = std::make_shared<ArbitrageCore>();
    PriceMt4DecoderPtr priceMt4DecoderPtr = std::make_shared<PriceMt4Decoder>();
    TradeMt4DecoderPtr tradeMt4DecoderPtr = std::make_shared<TradeMt4Decoder>();


    /********************************************************************/
    /*** MT4 IoStream ***************************************************/
    /********************************************************************/
    if ( result.count("mt4stream") )
    {
        priceMt4StreamPtr = std::make_shared<PriceMt4Pull>(
                    Config::getInstance()->price_mt4_pull_bind);
        priceMt4StreamPtr->Attach(priceMt4DecoderPtr);

        tradeMt4StreamPtr = std::make_shared<TradeMt4Pull>(
                    Config::getInstance()->trade_mt4_pull_bind);
        tradeMt4StreamPtr->Attach(tradeMt4DecoderPtr);

        mt4PublisherPtr = std::make_shared<TradeMt4Publisher>(
                    Config::getInstance()->trade_mt4_pub_bind);
    }
    /********************************************************************/
    /*** Simulation Demux Stream ****************************************/
    /********************************************************************/
    if ( result.count("demux") )
    {
        //mt4PublisherPtr = std::make_shared<SimulatorMt4Pub>();
        SimulatorMt4PubPtr simMt4PubPtr = std::make_shared<SimulatorMt4Pub>();
        simMt4PubPtr->Attach(tradeMt4DecoderPtr);
        mt4PublisherPtr = simMt4PubPtr;

        ///////////////////////////////////////////////////////////////
        /*tradeMt4StreamPtr = std::make_shared<TradeMt4Pull>(
                    Config::getInstance()->trade_mt4_pull_bind);
        tradeMt4StreamPtr->Attach(tradeMt4DecoderPtr);

        mt4PublisherPtr = std::make_shared<TradeMt4Publisher>(
                    Config::getInstance()->trade_mt4_pub_bind);*/
        ///////////////////////////////////////////////////////////////

        demuxMt4StreamPtr = std::make_shared<DemuxMt4Stream>(result["demux"].as<std::string>());
        demuxMt4StreamPtr->Attach(router::in_price, priceMt4DecoderPtr);
        demuxMt4StreamPtr->Attach(router::in_trade, tradeMt4DecoderPtr);
        //demuxMt4StreamPtr->Attach(router::out_trade, mt4PublisherPtr);

        priceMt4DecoderPtr->setRealTime(false);
        //priceMt4DecoderPtr->setSimulatioMode();
        //tradeMt4DecoderPtr->setSimulatioMode();
    }
    /********************************************************************/
    /*** Simulation MUX Recorder ****************************************/
    /********************************************************************/
    if ( result.count("mux")  )
    {
        muxMt4RecorderPtr = std::make_shared<MuxMt4Recorder>(result["mux"].as<std::string>(),1);
        if( result.count("mt4stream") ){
            priceMt4StreamPtr->Attach(muxMt4RecorderPtr);
            //tradeMt4StreamPtr->Attach(muxMt4RecorderPtr);
            //tradeMt4PublisherPtr->Attach(muxMt4RecorderPtr);
        }
        if ( result.count("demux") )
        {
            demuxMt4StreamPtr->Attach(router::in_price, muxMt4RecorderPtr);
            demuxMt4StreamPtr->Attach(router::in_trade, muxMt4RecorderPtr);
            demuxMt4StreamPtr->Attach(router::out_trade, muxMt4RecorderPtr);
        }
    }
    /********************************************************************/
    /*** Simulation TICK Recorder ***************************************/
    /********************************************************************/
    if ( result.count("tickmt4recorder") )
    {
        TickMt4Format tickMt4Format;
        //#ifdef BIN_TICK_MT4_RECORDER
        //tickMt4Format.binary = true;
        //#else
        tickMt4Format.symbols = {"GBPUSD"};
        tickMt4Format.minutesPerFile = 60;
        tickMt4Format.minuteToWrite = 10;
        //#endif
        tickMt4Format.path = result["muxmt4recorder"].as<std::string>();
        tickMt4RecorderPtr = std::make_shared<TickMt4Recorder>(tickMt4Format);
        priceMt4DecoderPtr->Attach(tickMt4RecorderPtr);
    }
    if(priceMt4StreamPtr != nullptr)
    {
        priceMt4StreamPtr->Attach(priceMt4DecoderPtr);
    }
    /********************************************************************/
    /*** ARBITRAGE CORE *************************************************/
    /********************************************************************/
    priceMt4DecoderPtr->Attach(arbitrageManagerPtr);
    arbitrageManagerPtr->Attach(arbitrageCorePtr);
    tradeMt4DecoderPtr->Attach(arbitrageCorePtr);
    arbitrageCorePtr->AttachMt4Publisher(mt4PublisherPtr);

    if(priceMt4DecoderPtr != nullptr) priceMt4DecoderPtr->Start(Config::getInstance()->pair_threads);
    if(priceMt4StreamPtr != nullptr) priceMt4StreamPtr->start();
    if(tradeMt4StreamPtr != nullptr) tradeMt4StreamPtr->start();
    if(demuxMt4StreamPtr != nullptr) demuxMt4StreamPtr->start();
    if(tickMt4StreamPtr != nullptr) tickMt4StreamPtr->start();

#ifdef FIXSERVER
    LOG_INFO << "Iniciando servidor FIX...";
    FixServer fixServer("config/fix_settings.txt", false);
    fixServer.start();
#endif

    while(true /*|| priceMt4StreamPtr->isWorking() || priceMt4DecoderPtr->isProcessing()*/){
        std::this_thread::sleep_for (std::chrono::milliseconds(500));
    }

    return EXIT_SUCCESS;
}

/* TODO
// Multiplicador de Volume -> NOK
// revisão/ajustes nos log e relatorios -> NOK
// review nas threads usadas, impl AsyncProcess com quantidade de threads -> NOK
// compilação usando release e otimização -> NOK

// Mql4 das comunicação de trades
*/
