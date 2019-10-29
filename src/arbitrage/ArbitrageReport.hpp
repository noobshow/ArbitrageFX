#ifndef ARBITRAGEREPORT_H
#define ARBITRAGEREPORT_H

#include <fstream>
#include <map>

#include "common/Singleton.hpp"
#include "common/AsyncThreadProcess.hpp"
#include "common/Types.hpp"
#include "common/defines.hpp"

class ArbitrageReport :
        public Singleton<ArbitrageReport>,
        public AsyncThreadProcess<arbitrage_trade_s>
{
private:
    std::map<std::string, double> profitMap;
    std::ofstream orderReport;
    std::ofstream tradeReport;
    std::string TimeToString(uint64_t timestamp);
    double BidToDouble(TickPtr tickPtr);
    double AskToDouble(TickPtr tickPtr);
    double PriceToDouble(std::string symbol, int32_t price);
    double getTotalProfitByKey(std::string symbol, double profit);
    std::string getBrokerPairKey(ArbitrageTradePtr arbTradePtr);

public:
    ArbitrageReport();
    virtual ~ArbitrageReport();
    void AsyncUpdate(ArbitrageTradePtr dataPtr);
};

#endif // ARBITRAGEREPORT_H
