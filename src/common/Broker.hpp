#ifndef BROKER_H
#define BROKER_H

#include <memory>
#include <set>
#include <unordered_map>
#include <mutex>
#include <cstring>

#include "common/Symbol.hpp"
#include "common/Types.hpp"
#include "common/Config.hpp"


typedef std::unordered_map<std::string, double> umap_strd;
// criar fee por broker/simbolo
static umap_strd fee_usd{ { "ICM", 7.0 }, { "FFS", 0.0 }, { "F4Y", 10.0 }, { "TW4", 6.0 }, { "ROB", 7.0 }  };

static umap_strd point_usd{
    {"XAU",0.01}, {"XTI",0.01}, {"XBR",0.01}, {"BTC",0.01},
    {"XAG",0.001}, {"HUF",0.001}, {"JPY",0.001},
    {"GBP",0.00001}, {"EUR",0.00001}, {"CAD",0.00001}, {"CHF",0.00001},
    {"AUD",0.00001}, {"NZD",0.00001}, {"SGD",0.00001}, {"DKK",0.00001},
    {"HKD",0.00001}, {"NOK",0.00001}, {"PLN",0.00001}, {"SEK",0.00001},
    {"TRY",0.00001}, {"ZAR",0.00001}, {"CNH",0.00001}, {"CZK",0.0001},
    {"MXN",0.00001}, {"RUB",0.00001}, {"THB",0.00001}
};

static std::set<std::string> usdSymbols = { "XAUUSD", "XTIUSD", "XBRUSD", "BTCUSD",
    "XAGUSD", "HUFUSD", "USDJPY", "GBPUSD", "EURUSD", "USDCAD", "USDCHF", "AUDUSD",
    "NZDUSD", "USDSGD", "USDDKK", "USDHKD", "USDNOK", "USDPLN", "USDSEK", "USDTRY",
    "USDZAR", "USDCNH", "USDCZK", "USDMXN", "USDRUB", "USDTHB" };

static umap_strd point_symbol{
    {"XAUUSD",0.01}, {"XTIUSD",0.01}, {"XBRUSD",0.01}, {"BTCUSD",0.01},
    {"XAGUSD",0.001}, {"USDHUF",0.001}, {"USDJPY",0.001},
    {"GBPUSD",0.00001}, {"EURUSD",0.00001}, {"USDCAD",0.00001}, {"USDCHF",0.00001},
    {"AUDUSD",0.00001}, {"NZDUSD",0.00001}, {"USDSGD",0.00001}, {"USDDKK",0.00001},
    {"USDHKD",0.00001}, {"USDNOK",0.00001}, {"USDPLN",0.00001}, {"USDSEK",0.00001},
    {"USDTRY",0.00001}, {"USDZAR",0.00001}, {"USDCNH",0.00001}, {"USDCZK",0.0001},
    {"USDMXN",0.00001}, {"USDRUB",0.00001}, {"USDTHB",0.00001}, {"AUDJPY",0.001},
    {"AUDNZD",0.00001}, {"AUDCHF",0.00001}, {"AUDCAD",0.00001}, {"CADCHF",0.00001},
    {"CADJPY",0.001}, {"CHFJPY",0.001}, {"EURAUD",0.00001}, {"EURCAD",0.00001},
    {"EURCHF",0.00001}, {"EURGBP",0.00001}, {"EURJPY",0.001}, {"EURNZD",0.00001},
    {"GBPAUD",0.00001}, {"GBPCHF",0.00001}, {"GBPJPY",0.001}, {"GBPNZD",0.00001},
    {"NZDCAD",0.00001}, {"GBPCAD",0.00001}, {"NZDCHF",0.00001}, {"NZDJPY",0.001}
};

class Broker
{
private:
    std::mutex mutex;
    std::unordered_map<std::string, SymbolPtr> symbols;
    std::mutex mutex_symbol;
public:
    static std::shared_ptr<Broker> refBroker;
    const std::string broker_;
    Broker(std::string broker) : broker_(broker) { }
    SymbolPtr getSymbol(std::string symbol);
    int getFeeInPoints(TickPtr tickPtr);
    double getPointUsdValue(std::string symbol);
};

typedef std::shared_ptr<Broker> BrokerPtr;

#endif // BROKER_H
