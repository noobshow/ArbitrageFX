//#ifndef PROTOCOLDEFINITIONS_H
//#define PROTOCOLDEFINITIONS_H

//#include <iostream>
//#include <string>
//#include <chrono>
//#include <memory>
//#include <atomic>
//#include <cstdint>
//#include <cstdlib>

//#include "common/loguru.hpp"
//#include "common/Service.hpp"

//namespace mt4 {

//using namespace std::chrono;

//const std::string sep = ",";
//const std::string end = "\r\n";

//static std::atomic<uint64_t> counterBrkSrcCmd(0);
//static std::atomic<uint64_t> counterArbSrcCmd(0);

///* Commands suffix types
// * _brkReq  -> (brk->arb) comando de solicitação enviado pelo broker, presisa ser respondida pela arbitragem
// * _brkResp -> (arb->brk) comando de resposta enviada pela arbitragem.
// * _brkUpd  -> (brk->arb) comando de atualização enviado pelo broker, não precisa ser respondida
// * _arbReq  -> (arb->brk) comando de solicitação enviado pela arbitragem, presisa ser respondida pelo broker
// * _arbResp -> (brk->arb) comando de resposta enviado pelo broker
// * _arbUpd  -> (arb->brk) comando de atualização enviada pela arbitragem, não precisa ser respondida
// */

//namespace id{
///* broker commands */
//const int TickMt4 = 1;

///* arbitrage commands */
////const int testConnTimeArbReq = 100;
////const int testConnTimeArbResp = 101;
////const int testOpenOrderTimeArbReq = 102;
////const int testOpenOrderTimeArbResp = 103;
////const int testCloseOrderTimeArbReq = 104;
////const int testCloseOrderTimeArbResp = 105;

//inline std::string toString(int cmdId){
//    std::string cmdName = "Invalid";
//    switch (cmdId) {
//    case 1: cmdName = "mt4_cmd_price_update"; break;
////    case 100: cmdName = "testConnTimeArbReq"; break;
////    case 101: cmdName = "testConnTimeArbResp"; break;
////    case 102: cmdName = "testOpenOrderTimeArbReq"; break;
////    case 103: cmdName = "testOpenOrderTimeArbResp"; break;
////    case 104: cmdName = "testCloseOrderTimeArbReq"; break;
////    case 105: cmdName = "testCloseOrderTimeArbResp"; break;
//    }
//    return cmdName;
//}
//}

///***************************************************************/
///*** Command 1 TickMt4 ***/
///*** timestamp;broker;symbol;ask;bid ***/
///***************************************************************/
//struct TickMt4
//{
//    uint64_t timestamp_;
//    std::string  broker_;
//    std::string  symbol_;
//    int bid_;
//    int ask_;
//    bool valid_;
//    TickMt4() {}
//    TickMt4(std::string timestamp, std::string broker, std::string symbol, std::string bid, std::string ask)
//    {
//        this->timestamp_ = static_cast<uint64_t>(atoll(timestamp.c_str()));
//        this->broker_ = broker;
//        this->symbol_ = symbol;
//        this->bid_ = atoi(bid.c_str());
//        this->ask_ = atoi(ask.c_str());

//        if( timestamp_ > 0 && ask_ > 0 && bid_ > 0 && broker_.size() == 3 && symbol_.size() == 6){
//            this->valid_ = true;
//        }
//        else{
//            valid_ = false;
//            LOG_ERROR << "Invalid TickMt4 (" << timestamp_ << ", " << broker_
//                      << ", " << symbol_ << ", " << ask_ << ", " << bid_ << ")";
//        }
//    }

//};

///***************************************************************/
///* generic command to broker */
///***************************************************************/
//class Command{
//public:
//    Command(int cmdId, std::string broker)
//        : commandID(cmdId), brokerName(broker)
//    {
//    }
//    int commandID;
//    uint64_t counter;
//    std::string brokerName;
//    ServicePtr servicePtr;
//};
///***************************************************************/
///* CommandTimer */
///***************************************************************/
//class CommandTimer{
//public:
//    std::string name;
//    const steady_clock::time_point mStart = steady_clock::now();
//    CommandTimer(std::string _name) : name(_name){ }
//    ~CommandTimer(){ this->stop(); }
//    void stop()
//    {
//        const auto end = steady_clock::now();
//        LOG_INFO << "CommandTimer " << this->name << ": "
//                  << duration<double, std::nano> (end - mStart).count() << " ns";
//    }
//};
///***************************************************************/
///* CommandToBroker */
///***************************************************************/
//class CommandToBroker : public Command{
//public:
//    CommandToBroker(int cmdId, std::string broker)
//        : Command(cmdId, broker) {
//        this->counter = counterArbSrcCmd++;
//    }
//    virtual ~CommandToBroker();
//    virtual std::string getCommand() = 0;
//};
///***************************************************************/
///* CommandFromBroker */
///***************************************************************/
//class CommandFromBroker : public Command{
//public:
//    bool valid;
//    CommandFromBroker(int cmdId, std::string broker)
//        : Command(cmdId, broker) {
//        this->valid = true;
//    }

//};

///***************************************************************/
///*** Command 5 UpdateSymbolBrkUpd ***/
///*** id;broker;symbol;ask;bid ***/
///***************************************************************/
////class UpdateSymbolBrkUpd : public CommandFromBroker, public CommandTimer{
////public:
////    std::string symbolName;
////    double ask;
////    double bid;
////    UpdateSymbolBrkUpd(std::string broker, std::string symbol, std::string ask, std::string bid)
////        : CommandFromBroker(id::TickMt4, broker), CommandTimer("UpdateSymbolBrkUpd")
////    {
////        try{
////            this->counter = counterBrkSrcCmd++;
////            this->symbolName = symbol;
////            this->ask = stod(ask);
////            this->bid = stod(bid);
////            this->valid = true;
////        }
////        catch(const std::invalid_argument& ia){
////            LOG_WARNING << "UpdateSymbol invalid_argument Exception: " << ia.what();
////            this->valid = false;
////        }
////        catch(const std::out_of_range& oor){
////            LOG_WARNING << "UpdateSymbol out_of_range Exception: " << oor.what();
////            this->valid = false;
////        }
////    }

////};

///***************************************************************/
///* Test commands definition */
///***************************************************************/
///*** Command 100 TestConnTimeArbReq ***/
///*** id;counter;broker ***/
///***************************************************************/
////class TestConnTimeArbReq : public CommandToBroker, public CommandTimer{
////public:
////    TestConnTimeArbReq(std::string broker)
////        : CommandToBroker(id::testConnTimeArbReq, broker), CommandTimer("TestConnTimeArbReq") {}
////    std::string getCommand() override {
////        return std::to_string(commandID)+sep+std::to_string(counter)+sep+brokerName+end;
////    }
////};

///***************************************************************/
///*** Command 101 TestConnTimeArbResp ***/
///*** id;counter;broker ***/
///***************************************************************/
////class TestConnTimeArbResp : public CommandFromBroker{
////public:
////    TestConnTimeArbResp(std::string count, std::string broker)
////        : CommandFromBroker(id::testConnTimeArbResp, broker)
////    {
////        try{
////            this->counter = std::stoull(count);
////        }
////        catch(const std::invalid_argument& ia){
////            LOG_WARNING << "TestConnTimeArbResp invalid_argument Exception: " << ia.what();
////            this->valid = false;
////        }
////        catch(const std::out_of_range& oor){
////            LOG_WARNING << "TestConnTimeArbResp out_of_range Exception: " << oor.what();
////            this->valid = false;
////        }
////    }
////};
///***************************************************************/
///*** Command 102 TestOpenOrderTimeArbReq ***/
///*** id;counter;broker ***/
///***************************************************************/
////class TestOpenOrderTimeArbReq : public CommandToBroker, public CommandTimer{
////public:
////    TestOpenOrderTimeArbReq(std::string broker)
////        : CommandToBroker(id::testOpenOrderTimeArbReq, broker), CommandTimer("TestOpenOrderTimeArbReq") {}
////    std::string getCommand() override {
////        return std::to_string(commandID)+sep+std::to_string(counter)+sep+brokerName+end;
////    }
////};
///***************************************************************/
///*** Command 103 TesOpentOrderTimeArbResp ***/
///*** id;counter;broker ***/
///***************************************************************/
////class TestOpenOrderTimeArbResp : public CommandFromBroker{
////public:
////    TestOpenOrderTimeArbResp(std::string count, std::string broker)
////        : CommandFromBroker(id::testOpenOrderTimeArbResp, broker)
////    {
////        try{
////            this->counter = std::stoull(count);
////        }
////        catch(const std::invalid_argument& ia){
////            LOG_WARNING << "TesOpentOrderTimeArbResp invalid_argument Exception: " << ia.what();
////            this->valid = false;
////        }
////        catch(const std::out_of_range& oor){
////            LOG_WARNING << "TesOpentOrderTimeArbResp out_of_range Exception: " << oor.what();
////            this->valid = false;
////        }
////    }
////};
///***************************************************************/
///*** Command 104 TestCloseOrderTimeArbReq ***/
///*** id;counter;broker ***/
///***************************************************************/
////class TestCloseOrderTimeArbReq : public CommandToBroker, public CommandTimer{
////public:
////    TestCloseOrderTimeArbReq(std::string broker)
////        : CommandToBroker(id::testCloseOrderTimeArbReq, broker), CommandTimer("TestCloseOrderTimeArbReq") {}
////    std::string getCommand() override {
////        return std::to_string(commandID)+sep+std::to_string(counter)+sep+brokerName+end;
////    }
////};
///***************************************************************/
///*** Command 105 TesCloseOrderTimeArbResp ***/
///*** id;counter;broker ***/
///***************************************************************/
////class TestCloseOrderTimeArbResp : public CommandFromBroker{
////public:
////    TestCloseOrderTimeArbResp(std::string count, std::string broker)
////        : CommandFromBroker(id::testCloseOrderTimeArbResp, broker)
////    {
////        try{
////            this->counter = std::stoull(count);
////        }
////        catch(const std::invalid_argument& ia){
////            LOG_WARNING << "TesOpentOrderTimeArbResp invalid_argument Exception: " << ia.what();
////            this->valid = false;
////        }
////        catch(const std::out_of_range& oor){
////            LOG_WARNING << "TesOpentOrderTimeArbResp out_of_range Exception: " << oor.what();
////            this->valid = false;
////        }
////    }
////};
///***************************************************************/
///* typedef pointers definition */
//typedef std::shared_ptr<Command> CommandPtr;
//typedef std::shared_ptr<CommandToBroker> CommandToBrokerPtr;
//typedef std::shared_ptr<CommandFromBroker> CommandFromBrokerPtr;
//typedef std::shared_ptr<TickMt4> TickPtr;
////typedef std::shared_ptr<TestConnTimeArbReq> TestConnTimeArbReqPtr;
////typedef std::shared_ptr<TestConnTimeArbResp> TestConnTimeArbRespPtr;
////typedef std::shared_ptr<TestOpenOrderTimeArbReq> TestOpenOrderTimeArbReqPtr;
////typedef std::shared_ptr<TestOpenOrderTimeArbResp> TestOpentOrderTimeArbRespPtr;
////typedef std::shared_ptr<TestCloseOrderTimeArbReq> TestCloseOrderTimeArbReqPtr;
////typedef std::shared_ptr<TestCloseOrderTimeArbResp> TesCloseOrderTimeArbRespPtr;
///***************************************************************/
//}

//#endif // PROTOCOLDEFINITIONS_H
