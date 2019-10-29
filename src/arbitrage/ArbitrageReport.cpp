#include "ArbitrageReport.hpp"
#include "common/Broker.hpp"

const std::string SEP = ";";


ArbitrageReport::ArbitrageReport()
{
#ifdef ORDER_REPORT
    orderReport.open("ArbOrderReport.csv", std::ofstream::out | std::ofstream::app);
    orderReport.imbue(std::locale("pt_BR.UTF-8"));
    orderReport << "TradeID" << SEP << "KEY" << SEP << "Symbol" << SEP << "Broker" << SEP << "Status" << SEP
                << "open_type" << SEP << "open_status" << SEP << "open_time" << SEP
                << "open_duration" << SEP << "Ticket" << SEP << "open_volume" << SEP << "open_price" << SEP
                << "opened_price" << SEP << "comission" << SEP << "slippage" << SEP << "spread_avg" << SEP
                << "spread_open" << SEP << "spread_expected" << SEP << "spread_close" << SEP
                << "close_status" << SEP << "close_time" << SEP << "close_price" << SEP << "closed_price" << SEP
                << "profit_points" << SEP << "profit_usd" << SEP << "profit_symbol" << SEP << "profit_broker"
                << std::endl;
#endif

#ifdef TRADE_REPORT
    tradeReport.open("ArbTradeReport.csv", std::ofstream::out | std::ofstream::app);
    tradeReport.imbue(std::locale("pt_BR.UTF-8"));
    tradeReport << "TradeID" << SEP << "KEY" << SEP << "Symbol" << SEP << "Status" << SEP
                << "Duration" << SEP
                << "Broker1" << SEP << "order1_type" << SEP << "open1_status" << SEP << ""
                << "Broker2" << SEP << "order2_type" << SEP << "open2_status" << SEP
                << "cost" << SEP << "gain_min" << SEP << "gain_match" << SEP  << "total_match" << SEP
                << "profit_points" << SEP << "profit_usd" << SEP
                << "profit_SymbolKey" << SEP << "profit_brokerPair"
                << std::endl;
#endif
}

ArbitrageReport::~ArbitrageReport()
{
    if(orderReport && orderReport.is_open()){
        orderReport.close();
    }
    if(tradeReport && tradeReport.is_open()){
        tradeReport.close();
    }
}

void ArbitrageReport::AsyncUpdate(ArbitrageTradePtr dataPtr)
{


#ifdef TRADE_REPORT
    tradeReport << dataPtr->trade_id_ << SEP
                << dataPtr->pair_key_ << SEP
                << dataPtr->symbol_ << SEP
                << TRADE_STR(dataPtr->trade_status_) << SEP
                << dataPtr->getDurationTrade() << SEP
                << dataPtr->openOrderArborPtr->broker_ << SEP
                << OP_STR(dataPtr->openOrderArborPtr->order_type_) << SEP
                << ORDER_STR(dataPtr->openOrderArborPtr->order_status_) << SEP
                << dataPtr->openOrderArbedPtr->broker_ << SEP
                << OP_STR(dataPtr->openOrderArbedPtr->order_type_) << SEP
                << ORDER_STR(dataPtr->openOrderArbedPtr->order_status_) << SEP
                << dataPtr->arbPointsPtr->totalCost << SEP
                << dataPtr->arbPointsPtr->gainMin << SEP
                << dataPtr->arbPointsPtr->gainMatch << SEP
                << dataPtr->arbPointsPtr->totalMatch << SEP
                << dataPtr->profit_points << SEP
                << dataPtr->profit_usd << SEP
                << this->getTotalProfitByKey(dataPtr->pair_key_, dataPtr->profit_usd) << SEP
                << this->getTotalProfitByKey(getBrokerPairKey(dataPtr), dataPtr->profit_usd)
                << std::endl;
#endif

#ifdef ORDER_REPORT
    orderReport << dataPtr->trade_id_ << SEP
                << dataPtr->pair_key_ << SEP
                << dataPtr->symbol_ << SEP
                << dataPtr->openOrderArborPtr->broker_ << SEP
                << TRADE_STR(dataPtr->trade_status_) << SEP
                << OP_STR(dataPtr->openOrderArborPtr->order_type_) << SEP
                << ORDER_STR(dataPtr->openOrderArborPtr->order_status_) << SEP
                << TimeToString(dataPtr->arbitrator_begin_tick->timestamp_) << SEP
                << dataPtr->getArborOrderDuration() << SEP
                << dataPtr->openOrderArborPtr->ticket_ << SEP
                << dataPtr->openOrderArborPtr->volume_ << SEP
                << PriceToDouble(dataPtr->symbol_, dataPtr->openOrderArborPtr->price_) << SEP
                << PriceToDouble(dataPtr->symbol_, dataPtr->openOrderArborPtr->opened_price_) << SEP
                << dataPtr->arbPointsPtr->fee1 << SEP
                << dataPtr->openOrderArborPtr->slippage_ << SEP
                << dataPtr->arbPointsPtr->spreadAvg1 << SEP
                << dataPtr->arbPointsPtr->spreadOpen1 << SEP
                << dataPtr->arbPointsPtr->spreadExpected1 << SEP
                << dataPtr->arbPointsPtr->spreadClose1 << SEP;
    if(dataPtr->closeOrderArborPtr)
    {
        orderReport << ORDER_STR(dataPtr->closeOrderArborPtr->order_status_) << SEP
                    << TimeToString(dataPtr->arbitrator_close_tick->timestamp_) << SEP
                    << PriceToDouble(dataPtr->symbol_, dataPtr->closeOrderArborPtr->price_) << SEP
                    << PriceToDouble(dataPtr->symbol_, dataPtr->closeOrderArborPtr->closed_price_) << SEP
                    << dataPtr->closeOrderArborPtr->profit_points_ << SEP
                    << dataPtr->closeOrderArborPtr->profit_usd_ << SEP
                    << this->getTotalProfitByKey(dataPtr->symbol_, dataPtr->closeOrderArborPtr->profit_usd_) << SEP
                    << this->getTotalProfitByKey(dataPtr->openOrderArborPtr->broker_, dataPtr->closeOrderArborPtr->profit_usd_);
    }
    else
    {
        orderReport << 0 << SEP << 0 << SEP << 0 << SEP << 0 << SEP << 0 << SEP << 0 << SEP << 0 << SEP << 0;
    }
    orderReport << std::endl;

    orderReport << dataPtr->trade_id_ << SEP
                << dataPtr->pair_key_ << SEP
                << dataPtr->symbol_ << SEP
                << dataPtr->openOrderArbedPtr->broker_ << SEP
                << TRADE_STR(dataPtr->trade_status_) << SEP
                << OP_STR(dataPtr->openOrderArbedPtr->order_type_) << SEP
                << ORDER_STR(dataPtr->openOrderArbedPtr->order_status_) << SEP
                << TimeToString(dataPtr->arbitrated_begin_tick->timestamp_) << SEP
                << dataPtr->getArbedOrderDuration() << SEP
                << dataPtr->openOrderArbedPtr->ticket_ << SEP
                << dataPtr->openOrderArbedPtr->volume_ << SEP
                << PriceToDouble(dataPtr->symbol_, dataPtr->openOrderArbedPtr->price_) << SEP
                << PriceToDouble(dataPtr->symbol_, dataPtr->openOrderArbedPtr->opened_price_) << SEP
                << dataPtr->arbPointsPtr->fee2 << SEP
                << dataPtr->openOrderArbedPtr->slippage_ << SEP
                << dataPtr->arbPointsPtr->spreadAvg2 << SEP
                << dataPtr->arbPointsPtr->spreadOpen2 << SEP
                << dataPtr->arbPointsPtr->spreadExpected2 << SEP
                << dataPtr->arbPointsPtr->spreadClose2 << SEP;
    if(dataPtr->closeOrderArbedPtr)
    {
        orderReport << ORDER_STR(dataPtr->closeOrderArbedPtr->order_status_) << SEP
                    << TimeToString(dataPtr->arbitrated_close_tick->timestamp_) << SEP
                    << PriceToDouble(dataPtr->symbol_, dataPtr->closeOrderArbedPtr->price_) << SEP
                    << PriceToDouble(dataPtr->symbol_, dataPtr->closeOrderArbedPtr->closed_price_) << SEP
                    << dataPtr->closeOrderArbedPtr->profit_points_ << SEP
                    << dataPtr->closeOrderArbedPtr->profit_usd_ << SEP
                    << this->getTotalProfitByKey(dataPtr->symbol_, dataPtr->closeOrderArbedPtr->profit_usd_) << SEP
                    << this->getTotalProfitByKey(dataPtr->openOrderArbedPtr->broker_, dataPtr->closeOrderArbedPtr->profit_usd_);
    }
    else{
        orderReport << 0 << SEP << 0 << SEP << 0 << SEP << 0 << SEP << 0 << SEP << 0 << SEP << 0 << SEP << 0;
    }
    orderReport << std::endl;
#endif

}

std::string ArbitrageReport::TimeToString(uint64_t timestamp)
{
    std::stringstream ss;
    auto in_time_t = TimestampToTime_t(timestamp);
    ss <<  std::put_time(std::localtime(&in_time_t), "%Y-%m-%d_%X");
    ss << "." << (timestamp % 1000);
    return ss.str();
}

double ArbitrageReport::BidToDouble(TickPtr tickPtr)
{
    return point_symbol[tickPtr->symbol_] * static_cast<double>(tickPtr->bid_);
}

double ArbitrageReport::AskToDouble(TickPtr tickPtr)
{
    return point_symbol[tickPtr->symbol_] * static_cast<double>(tickPtr->ask_);
}

double ArbitrageReport::PriceToDouble(std::string symbol, int32_t price)
{
    return point_symbol[symbol] * static_cast<double>(price);
}

std::string ArbitrageReport::getBrokerPairKey(ArbitrageTradePtr arbTradePtr)
{
    std::string key;
    if( arbTradePtr->arbitrator_begin_tick->broker_ < arbTradePtr->arbitrated_begin_tick->broker_){
        key = key + arbTradePtr->arbitrator_begin_tick->broker_ + arbTradePtr->arbitrated_begin_tick->broker_;
    }
    else{
        key = key + arbTradePtr->arbitrated_begin_tick->broker_ + arbTradePtr->arbitrator_begin_tick->broker_;
    }
    return key;
}

double ArbitrageReport::getTotalProfitByKey(std::string symbol, double profit)
{
    double totalProfit = 0;
    auto it = this->profitMap.find(symbol);
    if( it == this->profitMap.end() ){
        totalProfit = profit;
        this->profitMap[symbol] = totalProfit;
    }
    else
    {
        it->second = it->second + profit;
        totalProfit = it->second;
    }
    return totalProfit;
}

