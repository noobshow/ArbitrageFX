#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>
#include <memory>
#include <map>
#include <list>
#include <mutex>

#include "common/Types.hpp"

struct price_s{
    int bid;
    int ask;
};

typedef std::pair<uint32_t, uint32_t> price_p;
typedef uint64_t timestamp_t;

class Symbol
{
private:
    TickPtr lastTickPtr;
    std::list<double> spread_buffer_;
    std::map<timestamp_t, price_s> ticks;
    std::mutex mutex_update_;
    uint32_t max_buffer_size_;
    uint32_t spread_buffer_size_;
    double spread_buffer_sum_;
    double spread_buffer_avg_;
public:
    const std::string symbol_;
    const std::string broker_;

    Symbol(std::string symbol, std::string broker);
    void updateSymbol(TickPtr tickPtr);
    int32_t getSpreadAvg();
    TickPtr getLastTick();
};

typedef std::shared_ptr<Symbol> SymbolPtr;

#endif // SYMBOL_H
