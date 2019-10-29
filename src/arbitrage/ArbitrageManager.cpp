#include <cstring>
#include "arbitrage/ArbitrageManager.hpp"


ArbitrageManager::ArbitrageManager()
{
}

void ArbitrageManager::Update(TickPtr tickPtr)
{

    BrokerPtr brokerPtr;

    this->mutex_broker_index.lock();
    auto it_broker = this->brokerIndexMap.find(tickPtr->broker_);
    if(it_broker ==  this->brokerIndexMap.end())
    {
        brokerPtr = std::make_shared<Broker>(tickPtr->broker_);
        this->brokerIndexMap.insert(std::pair<std::string, BrokerPtr>(tickPtr->broker_, brokerPtr));
        if( 0 == strcmp(brokerPtr->broker_.c_str(), Config::getInstance()->referenceBroker.c_str()))
        {
            Broker::refBroker = brokerPtr;
        }
    }
    else{
        brokerPtr = it_broker->second;
    }
    this->mutex_broker_index.unlock();

    SymbolPtr symbolPtr = brokerPtr->getSymbol(tickPtr->symbol_);
    symbolPtr->updateSymbol(tickPtr);
    int32_t spread_avg1 = symbolPtr->getSpreadAvg();

    SymbolUMapPtr symbolUMapPtr = nullptr;

    this->mutex_symbol_index.lock();
    auto it = this->symbolIndexMap.find( tickPtr->symbol_ );
    if(it ==  this->symbolIndexMap.end())
    {
        symbolUMapPtr = std::make_shared<symbol_umap_s>();
        symbolUMapPtr->umap.insert(std::make_pair( brokerPtr->broker_, symbolPtr ));
        this->symbolIndexMap.insert( std::make_pair(tickPtr->symbol_, symbolUMapPtr));
        this->mutex_symbol_index.unlock();
    }
    else
    {
        symbolUMapPtr = it->second;
        this->mutex_symbol_index.unlock();

        for( auto it : symbolUMapPtr->umap )
        {
            if( 0 != strcmp(tickPtr->broker_.c_str(), it.first.c_str()))
            {
                int32_t spread_avg2 = it.second->getSpreadAvg();
                ArbitrageUpdatePtr arbUpdatePtr =
                        std::make_shared<arbitrage_update_s>(tickPtr, it.second->getLastTick(), spread_avg1, spread_avg2);

                arbitrageCorePtr->UpdatePair(arbUpdatePtr);
            }
        }
    }
}

