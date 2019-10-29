#ifndef ARBITRAGEROUTER_H
#define ARBITRAGEROUTER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <thread>
#include <mutex>

#include "arbitrage/ArbitrageCore.hpp"
#include "common/Observer.hpp"
#include "common/Router.hpp"
#include "common/Types.hpp"
#include "common/Broker.hpp"

typedef std::pair<std::string, BrokerPtr> broker_pair;

struct symbol_umap_s{
    std::unordered_map<std::string, SymbolPtr> umap;
};
typedef std::shared_ptr<symbol_umap_s>SymbolUMapPtr;


class ArbitrageManager : public Observer<tick_s>
{
private:
    std::unordered_map<std::string, BrokerPtr> brokerIndexMap;
    std::unordered_map<std::string, SymbolUMapPtr> symbolIndexMap;
    std::mutex mutex_broker_index;
    std::mutex mutex_symbol_index;

    ArbitrageCorePtr arbitrageCorePtr;

public:
    ArbitrageManager();
    void Update(TickPtr tickPtr);

    void Attach(ArbitrageCorePtr arbCorePtr) { this->arbitrageCorePtr = arbCorePtr; }
};

typedef std::shared_ptr<ArbitrageManager> ArbitrageManagerPtr;

#endif // ARBITRAGEROUTER_H
