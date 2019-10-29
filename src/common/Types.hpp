#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <string>
#include <memory>
#include <atomic>
#include <map>
#include <vector>

#include "common/loguru.hpp"
#include "common/Utils.hpp"
#include "common/Singleton.hpp"
//#include "common/Broker.hpp"

const std::string DELIMITER = ",";

static int32_t ORDER_ID = 1;
static int32_t TRADE_ID = 1;

/********************************************************/
/*** COMMAND DEFINITIONS                                */
/********************************************************/
static const int32_t CMD_PRICE = 1;
static const int32_t CMD_OPEN_ORDER_REQ = 2;
static const int32_t CMD_OPEN_ORDER_RESP = 3;
static const int32_t CMD_CLOSE_ORDER_REQ = 4;
static const int32_t CMD_CLOSE_ORDER_RESP = 5;

inline std::string CMD_STR(int32_t value){
    switch (value) {
    case CMD_PRICE: return "CMD_PRICE";
    case CMD_OPEN_ORDER_REQ: return "CMD_OPEN_ORDER_REQ";
    case CMD_OPEN_ORDER_RESP: return "CMD_OPEN_ORDER_RESP";
    case CMD_CLOSE_ORDER_REQ: return "CMD_CLOSE_ORDER_REQ";
    case CMD_CLOSE_ORDER_RESP: return "CMD_CLOSE_ORDER_RESP";
    } return "CMD_INVALID";
}
/********************************************************/
/*** ORDER STATUS DEFINITIONS                           */
/********************************************************/
static const int32_t ORDER_CREATED = 1;
static const int32_t ORDER_SEND = 2;
static const int32_t ORDER_FILLED = 3;
static const int32_t ORDER_ERROR = 4;
static const int32_t ORDER_TIMEOUT = 5;
static const int32_t ORDER_DISABLED = 6;

inline std::string ORDER_STR(int32_t value){
    switch (value) {
    case ORDER_CREATED: return "ORDER_CREATED";
    case ORDER_SEND: return "ORDER_SEND";
    case ORDER_FILLED: return "ORDER_FILLED";
    case ORDER_ERROR: return "ORDER_ERROR";
    case ORDER_TIMEOUT: return "ORDER_TIMEOUT";
    case ORDER_DISABLED: return "ORDER_DISABLED";
    } return "ORDER_INVALID";
}
/********************************************************/
/*** TRADE STATUS DEFINITIONS                           */
/********************************************************/
static const int32_t TRADE_CREATED = 1;
static const int32_t TRADE_OPENING = 2;
static const int32_t TRADE_OPENED = 3;
static const int32_t TRADE_PARTIALLY_OPENED = 4;
static const int32_t TRADE_CLOSING = 5;
static const int32_t TRADE_CLOSED = 6;
static const int32_t TRADE_OPEN_ERROR = 7;
static const int32_t TRADE_CLOSE_ERROR = 8;

inline std::string TRADE_STR(int32_t value){
    switch (value) {
    case TRADE_CREATED: return "TRADE_CREATED";
    case TRADE_OPENING: return "TRADE_OPENING";
    case TRADE_OPENED: return "TRADE_OPENED";
    case TRADE_PARTIALLY_OPENED: return "TRADE_PARTIALLY_OPENED";
    case TRADE_CLOSING: return "TRADE_CLOSING";
    case TRADE_CLOSED: return "TRADE_CLOSED";
    case TRADE_OPEN_ERROR: return "TRADE_OPEN_ERROR";
    case TRADE_CLOSE_ERROR: return "TRADE_CLOSE_ERROR";
    } return "TRADE_INVALID";
}
/********************************************************/
/*** ORDER TYPE DEFINITIONS                             */
/********************************************************/
static const int32_t OP_UNDEFINED = -1;
static const int32_t OP_BUY = 0;
static const int32_t OP_SELL = 1;
static const int32_t OP_BUYLIMIT = 2;
static const int32_t OP_SELLLIMIT = 3;
static const int32_t OP_BUYSTOP = 4;
static const int32_t OP_SELLSTOP = 5;

inline std::string OP_STR(int32_t value){
    switch (value) {
    case OP_UNDEFINED: return "OP_UNDEFINED";
    case OP_BUY: return "OP_BUY";
    case OP_SELL: return "OP_SELL";
    case OP_BUYLIMIT: return "OP_BUYLIMIT";
    case OP_SELLLIMIT: return "OP_SELLLIMIT";
    case OP_BUYSTOP: return "OP_BUYSTOP";
    case OP_SELLSTOP: return "OP_SELLSTOP";
    } return "OP_INVALID";
}
/********************************************************/
/*** ROUTER DEFINITIONS                                 */
/********************************************************/
namespace router
{
const std::string match_pair_open = "MPO";
const std::string match_pair_close = "MPC";

const std::string in_price = "IP";
const std::string in_trade = "IT";
const std::string out_trade = "OT";
}

/********************************************************/
/*** ARBITRAGE DEFINITIONS                              */
/********************************************************/
static const int32_t ARB_UNDEF = 0;
static const int32_t ARBITRATOR_BUY = 1;
static const int32_t ARBITRATOR_SELL = 2;
static const int32_t ARB_SLOW = 3;
static const int32_t ARB_FAST = 4;

inline std::string ARB_STR(int32_t value){
    switch (value) {
    case ARBITRATOR_BUY: return "ARBITRATOR_BUY";
    case ARBITRATOR_SELL: return "ARBITRATOR_SELL";
    case ARB_SLOW: return "ARB_SLOW";
    case ARB_FAST: return "ARB_FAST";
    } return "ARB_INVALID";
}

namespace arb
{
enum status { OPEN, CLOSE };
}

//class Global : public Singleton<Global> {
//public:
//    uint32_t ORDER_ID = 0;
//    uint32_t ARBITRAGE_ID = 0;
//    uint32_t getNewOrderId() { return ORDER_ID++; }
//    uint32_t getNewArbitrageId() { return ARBITRAGE_ID++; }
//};

/***************************************************************/
/*** Tick Struct ***/
/*** timestamp;broker;symbol;ask;bid ***/
/***************************************************************/
struct tick_s
{
    uint64_t timestamp_;
    const std::string  broker_;
    const std::string  symbol_;
    const int32_t bid_;
    const int32_t ask_;
    bool valid_;

    tick_s(std::string timestamp, std::string broker, std::string symbol, std::string bid, std::string ask) :
        timestamp_(static_cast<uint64_t>(atoll(timestamp.c_str()))),
        broker_(broker),
        symbol_(symbol),
        bid_(atoi(bid.c_str())),
        ask_(atoi(ask.c_str()))

    {
        if( timestamp_ > 0 && ask_ > 0 && bid_ > 0 && broker_.size() == 3 && symbol_.size() == 6){
            this->valid_ = true;
        }
        else{
            valid_ = false;
            LOG_ERROR << "Invalid Tick (" << timestamp_ << ", " << broker_
                      << ", " << symbol_ << ", " << ask_ << ", " << bid_ << ")";
        }
    }
    std::string to_string(){
        std::stringstream ss;
        ss << TimestampToFormattedString(timestamp_) << " "
           << broker_ << " " << symbol_ << " " << bid_ << " " << ask_;
        return ss.str();
    }
    int32_t getCenterPrice(){
        return ((bid_+ask_) / 2);
    }

};
typedef std::shared_ptr<tick_s> TickPtr;

/*************************************************************************/
/*** Struct de preços para atualização de arbitragem                     */
/*************************************************************************/
struct arbitrage_update_s
{
    TickPtr tickArbitratorPtr;
    TickPtr tickArbitratedPtr;
    int32_t spreadAvgArbitrator;
    int32_t spreadAvgArbitrated;
    int32_t priceCenter1;
    int32_t priceCenter2;
    int32_t priceCenterDiff;
    int32_t spread1;
    int32_t spread2;

    arbitrage_update_s() {}
    arbitrage_update_s(TickPtr tickArbitratorPtr, TickPtr tickArbitratedPtr,
                       int32_t spreadAvgArbor, int32_t spreadAvgArbed)
        : tickArbitratorPtr(tickArbitratorPtr),
          tickArbitratedPtr(tickArbitratedPtr),
          spreadAvgArbitrator(spreadAvgArbor),
          spreadAvgArbitrated(spreadAvgArbed),
          priceCenter1(0),
          priceCenter2(0),
          priceCenterDiff(0),
          spread1(0),
          spread2(0)
    {}
    void calculateCenterAndSpread(){
        this->priceCenter1 = this->tickArbitratorPtr->getCenterPrice();
        this->priceCenter2 = this->tickArbitratedPtr->getCenterPrice();
        this->priceCenterDiff = this->priceCenter1 - this->priceCenter2;
        this->spread1 = this->tickArbitratorPtr->ask_ - this->tickArbitratorPtr->bid_;
        this->spread2 = this->tickArbitratedPtr->ask_ - this->tickArbitratedPtr->bid_;
    }
    int32_t getMaxArborSpread(){
        if( spread1 >= spreadAvgArbitrator )
            return spread1;
        return spreadAvgArbitrator;
    }
    int32_t getMaxArbedSpread(){
        if( spread2 >= spreadAvgArbitrated )
            return spread2;
        return spreadAvgArbitrated;
    }
    int32_t getExpectedArborSpread(){
        if( spreadAvgArbitrator >= spread1 )
            return spreadAvgArbitrator;
        return ( spreadAvgArbitrator + spread1 ) / 2;
    }
    int32_t getExpectedArbedSpread(){
        if( spreadAvgArbitrated >= spread2 )
            return spreadAvgArbitrated;
        return ( spreadAvgArbitrated + spread2 ) / 2;
    }
};
typedef std::shared_ptr<arbitrage_update_s> ArbitrageUpdatePtr;
/*************************************************************************/
/*** Struct de preços para atualização de arbitragem Triangular          */
/*************************************************************************/
struct triangle_update_s
{
    TickPtr tickPtr;
    //std::map<std::string, TickPtr> lastTickByBroker;

    triangle_update_s() {}
    triangle_update_s(TickPtr tickPtr) : tickPtr(tickPtr) {}
};
typedef std::shared_ptr<triangle_update_s> TriangleUpdatePtr;
/*************************************************************************/
/*** Struct de um comando de requisição generico  */
/*************************************************************************/
struct request_command
{
    int32_t command_id_;
    std::string broker_;
    request_command(int32_t command_id, std::string broker) :
        command_id_(command_id),
        broker_(broker)
    {}
    virtual std::string to_cmd_string() = 0;
    virtual ~request_command() {}
};
typedef std::shared_ptr<request_command> RequestCommandPtr;
/*************************************************************************/
/*** Struct da requisição de abertura de ordem / CMD_OPEN_ORDER_REQ */
/*************************************************************************/
struct open_order_cmd_s : public request_command
{
    int32_t order_id_;
    int32_t trade_id_;
    std::string symbol_;
    int32_t order_type_;
    double volume_;
    int32_t price_;
    int32_t slippage_;
    int32_t stoploss_;
    int32_t takeprofit_;
    int32_t order_status_;
    int32_t opened_price_;
    int32_t try_close_;
    int32_t ticket_;
    ~open_order_cmd_s() = default;
    open_order_cmd_s(std::string broker, int32_t trade_id, std::string symbol,
                     int32_t order_type, double volume, int32_t price,
                     int32_t slippage = 0, int32_t stoploss = 0, int32_t takeprofit = 0)
        : request_command(CMD_OPEN_ORDER_REQ, broker), trade_id_(trade_id), symbol_(symbol),
          order_type_(order_type), volume_(volume), price_(price), slippage_(slippage),
          stoploss_(stoploss), takeprofit_(takeprofit), ticket_(-1)
    {
        this->order_id_ = ORDER_ID++;
        order_status_ = ORDER_CREATED;
        opened_price_ = 0;
        try_close_ = 0;
    }

    std::string to_cmd_string(){
        std::stringstream ss;
        ss << CMD_OPEN_ORDER_REQ << DELIMITER << order_id_ << DELIMITER << trade_id_
           << DELIMITER << broker_ << DELIMITER << symbol_ << DELIMITER << order_type_
           << DELIMITER << volume_ << DELIMITER << price_ << DELIMITER << slippage_
           << DELIMITER << stoploss_ << DELIMITER << takeprofit_;
        return ss.str();
    }

    std::string to_print(){
        std::stringstream ss;
        ss << order_id_ << DELIMITER << trade_id_  << DELIMITER << ticket_
           << DELIMITER << broker_ << DELIMITER << symbol_ << DELIMITER << OP_STR(order_type_)
           << DELIMITER << volume_ << DELIMITER << price_ << DELIMITER << slippage_
           << DELIMITER << ORDER_STR(order_status_);
        return ss.str();
    }
};
typedef std::shared_ptr<open_order_cmd_s> OpenOrderCmdPtr;
/*************************************************************************/
/*** Struct da requisição de abertura de ordem / CMD_OPEN_ORDER_REQ */
/*************************************************************************/
struct close_order_cmd_s : public request_command
{
    int32_t order_id_;
    int32_t trade_id_;
    int32_t ticket_;
    std::string symbol_;
    double volume_;
    int32_t price_;
    int32_t slippage_;
    int32_t order_status_ = ORDER_CREATED;
    int32_t closed_price_ = 0;
    int32_t profit_points_ = 0;
    double profit_usd_ = 0;
    ~close_order_cmd_s() = default;
    close_order_cmd_s(std::string broker, int32_t trade_id, int32_t ticket,
                      std::string symbol, double volume, int32_t price, int32_t slippage = 0)
        : request_command(CMD_CLOSE_ORDER_REQ, broker), trade_id_(trade_id), ticket_(ticket),
          symbol_(symbol), volume_(volume), price_(price), slippage_(slippage)
    {
        this->order_id_ = ORDER_ID++;
    }

    std::string to_cmd_string(){
        std::stringstream ss;
        ss << CMD_CLOSE_ORDER_REQ << DELIMITER << order_id_ << DELIMITER << trade_id_
           << DELIMITER << ticket_ << DELIMITER << broker_ << DELIMITER << symbol_
           << DELIMITER << volume_ << DELIMITER << price_ << DELIMITER << slippage_;
        return ss.str();
    }
    std::string to_print(){
        std::stringstream ss;
        ss << order_id_ << DELIMITER << trade_id_ << DELIMITER  << ticket_ << DELIMITER
           << broker_ << DELIMITER << symbol_  << DELIMITER << volume_ << DELIMITER
           << price_ << DELIMITER << slippage_ << DELIMITER << ORDER_STR(order_status_);
        return ss.str();
    }
};
typedef std::shared_ptr<close_order_cmd_s> CloseOrderCmdPtr;

/***********************************************************************/
/*** Struct com informações dos calculos de arbitragem                 */
/***********************************************************************/
struct arbitrage_points_s
{
    int32_t trade_id;
    std::string pair_key;
    int32_t priceCenter1;
    int32_t priceCenter2;
    int32_t priceDiff;
    int32_t spreadAvg1;
    int32_t spreadAvg2;
    int32_t spreadOpen1;
    int32_t spreadOpen2;
    int32_t spreadExpected1;
    int32_t spreadExpected2;
    int32_t spreadClose1;
    int32_t spreadClose2;
    int32_t fee1;
    int32_t fee2;
    int32_t gainMin;
    int32_t gainMatch;
    int32_t totalCost;
    int32_t totalMatch;

    //arbitrage_points_s() { }
    arbitrage_points_s(int32_t trade_id, std::string pair_key,
                       int32_t priceCenter1, int32_t priceCenter2, int32_t priceDiff,
                       int32_t spreadAvg1, int32_t spreadAvg2,
                       int32_t spreadOpen1, int32_t spreadOpen2,
                       int32_t spreadExpected1, int32_t spreadExpected2,
                       int32_t spreadClose1, int32_t spreadClose2,
                       int32_t fee1, int32_t fee2,
                       int32_t gainMin, int32_t gainMatch, int32_t totalCost, int32_t totalMatch) :
        trade_id(trade_id), pair_key(pair_key),
        priceCenter1(priceCenter1), priceCenter2(priceCenter2), priceDiff(priceDiff),
        spreadAvg1(spreadAvg1), spreadAvg2(spreadAvg2),
        spreadOpen1(spreadOpen1), spreadOpen2(spreadOpen2),
        spreadExpected1(spreadExpected1), spreadExpected2(spreadExpected2),
        spreadClose1(spreadClose1), spreadClose2(spreadClose2),
        fee1(fee1), fee2(fee2),
        gainMin(gainMin), gainMatch(gainMatch), totalCost(totalCost), totalMatch(totalMatch)
    { }

    std::string to_string(){
        std::stringstream ss;
        ss << "ARB POINTS-> TradeId: " << trade_id << ", key: " << pair_key
           << ", priceC1: " << priceCenter1 << ", priceC2:" << priceCenter2 << ", priceDiff: " << priceDiff
           << ", spread1: " << spreadAvg1 << ", spread2: " << spreadAvg2
           << ", spreadO1: " << spreadOpen1 << ", spreadO2: " << spreadOpen2
           << ", spreadE1: " << spreadExpected1 << ", spreadE2: " << spreadExpected2
           << ", spreadC1: " << spreadClose1 << ", spreadC2: " << spreadClose2
           << ", fee1: " << fee1 << ", fee2: " << fee2
           << ", gainMin: " << gainMin << ", gainMatch: " << gainMatch
           << ", Cost: " << totalCost << ", totalMatch: " << totalMatch;
        return ss.str();
    }
};
typedef std::shared_ptr<arbitrage_points_s> ArbitragePointsPtr;
/***********************************************************************/
/*** Struct com informações de um trade                                */
/***********************************************************************/
struct arbitrage_trade_s
{
    int32_t trade_id_;
    std::string symbol_;
    std::string pair_key_;
    int32_t trade_status_ = TRADE_CREATED;
    int32_t arbitrator_side_ = ARB_UNDEF;
    TickPtr arbitrator_begin_tick;
    TickPtr arbitrated_begin_tick;
    OpenOrderCmdPtr openOrderArborPtr;
    OpenOrderCmdPtr openOrderArbedPtr;
    TickPtr arbitrator_close_tick;
    TickPtr arbitrated_close_tick;
    CloseOrderCmdPtr closeOrderArborPtr;
    CloseOrderCmdPtr closeOrderArbedPtr;
    ArbitragePointsPtr arbPointsPtr;
    uint64_t timestamp_open = 0;
    uint64_t timestamp_close = 0;
    int32_t profit_points = 0;
    double profit_usd = 0.0;

    arbitrage_trade_s() {
        trade_id_ = TRADE_ID++;
    }
    std::string to_key_string(){
        std::stringstream ss;
        ss << "Trade id: " << trade_id_ << ", key: " << pair_key_ << ".";
        return ss.str();
    }
    bool isArborOrderCompleted(){
        return (openOrderArborPtr && openOrderArborPtr->order_status_ == ORDER_FILLED &&
                closeOrderArborPtr && closeOrderArborPtr->order_status_ == ORDER_FILLED);
    }
    bool isArbedOrderCompleted(){
        return (openOrderArbedPtr && openOrderArbedPtr->order_status_ == ORDER_FILLED &&
                closeOrderArbedPtr && closeOrderArbedPtr->order_status_ == ORDER_FILLED);
    }
    double getArborOrderDuration(){
        double duration = 0.0;
        if(arbitrator_begin_tick && arbitrator_close_tick && isArborOrderCompleted()){
            duration = arbitrator_close_tick->timestamp_ - arbitrator_begin_tick->timestamp_;
        }
        return duration/1000.0;
    }
    double getArbedOrderDuration(){
        double duration = 0.0;
        if(arbitrated_begin_tick && arbitrated_close_tick && isArbedOrderCompleted()){
            duration = arbitrated_close_tick->timestamp_ - arbitrated_begin_tick->timestamp_;
        }
        return duration/1000.0;
    }
    double getDurationTrade(){
        double duration = 0.0;
        if(timestamp_close > 0){
            duration = static_cast<double>(timestamp_close - timestamp_open);
        }
        return duration;
    }

};
typedef std::shared_ptr<arbitrage_trade_s> ArbitrageTradePtr;

/***********************************************************************/
/*** Struct com informações dos trades de um par                       */
/***********************************************************************/
struct arbitrage_pair_s
{
    std::string key_;
    std::vector<ArbitrageTradePtr> tradesList;

    arbitrage_pair_s(std::string key) :
        key_(key)
    { }
};
typedef std::shared_ptr<arbitrage_pair_s> ArbitragePairPtr;

/***********************************************************************/
/*** Struct da reposta de abertura de ordem / CMD_OPEN_ORDER_RESP */
/***********************************************************************/
struct order_resp_cmd_s
{
    int32_t cmd_type_;
    int32_t order_id_;
    int32_t trade_id_;
    int32_t ticket_;
    int32_t order_status_;
    int32_t price_;
    bool valid_;

    order_resp_cmd_s(int32_t cmd_type, std::string order_id, std::string trade_id,
                     std::string ticket, std::string status, std::string price)
    {
        this->cmd_type_ = cmd_type;
        this->order_id_ = atoi(order_id.c_str());
        this->trade_id_ = atoi(trade_id.c_str());
        this->ticket_ = atoi(ticket.c_str());
        this->order_status_ = atoi(status.c_str());;
        this->price_ = atoi(price.c_str());

        if(cmd_type_ > 0 && order_id_ > 0 && trade_id_ > 0 && order_status_ > 0 && price_ > 0){
            this->valid_ = true;
        }
        else{
            valid_ = false;
            LOG_ERROR << "Invalid order_resp_cmd_s (" << cmd_type_ << ", " << order_id_ << ", "
                      << trade_id_ << ", " << order_status_ << ", " << price_  << ")";
        }
    }
    std::string to_string(){
        std::stringstream ss;
        ss << cmd_type_ << DELIMITER << order_id_ << DELIMITER << trade_id_
           << DELIMITER << order_status_ << DELIMITER << price_;
        return ss.str();
    }
    std::string to_print(){
        std::stringstream ss;
        ss << CMD_STR(cmd_type_) << DELIMITER << order_id_ << DELIMITER << trade_id_
           << DELIMITER << ORDER_STR(order_status_) << DELIMITER << price_;
        return ss.str();
    }
};
typedef std::shared_ptr<order_resp_cmd_s> OrderRespCmdPtr;

/***************************************************************/
#endif // TYPES_H
