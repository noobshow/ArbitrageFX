
#include <sstream>
#include <cmath>

#include "common/Broker.hpp"
#include "common/Utils.hpp"
#include "common/loguru.hpp"

std::shared_ptr<Broker> Broker::refBroker;

SymbolPtr Broker::getSymbol(std::string symbol)
{
    SymbolPtr symbolPtr;
    this->mutex_symbol.lock();
    auto it = this->symbols.find(symbol);
    if(it ==  this->symbols.end()){
        symbolPtr = std::make_shared<Symbol>(symbol, this->broker_);
        this->symbols.insert(std::pair<std::string, SymbolPtr>(symbol, symbolPtr));
    }
    else{
        symbolPtr = it->second;
    }
    this->mutex_symbol.unlock();
    return symbolPtr;
}

double Broker::getPointUsdValue(std::string symbol)
{
    double point_usd = 0.01;

    std::string currency = symbol.substr(3,3);

    if(currency != "USD")
    {
        std::string symbol = currency + "USD";
        auto it = this->symbols.find(symbol);
        if(it !=  this->symbols.end())
        {
            double price = 0.00001 * double(it->second->getLastTick()->bid_);
            point_usd = point_usd / price;
        }
        else{
            symbol = "USD" + currency;
            auto it = this->symbols.find(symbol);
            if(it !=  this->symbols.end())
            {
                double price = 0.00001 * double(it->second->getLastTick()->bid_);
                point_usd = point_usd * price;
            }
            else{
                LOG_ERROR << "symbol " << currency << "-USD not found for commission calculation." ;
            }
        }
    }
    return point_usd;
}

int Broker::getFeeInPoints(TickPtr tickPtr)
{
    int points = 0;

    double fee = fee_usd[tickPtr->broker_];
    if(fee > 0)
    {
        std::string currency = tickPtr->symbol_.substr(3,3);

        if(currency != "USD")
        {
            std::string symbol = currency + "USD";
            auto it = this->symbols.find(symbol);
            if(it !=  this->symbols.end())
            {
                double price = 0.00001 * double(it->second->getLastTick()->bid_);
                fee = fee / price;
            }
            else{
                symbol = "USD" + currency;
                auto it = this->symbols.find(symbol);
                if(it !=  this->symbols.end())
                {
                    double price = 0.00001 * double(it->second->getLastTick()->bid_);
                    fee = fee * price;
                }
                else{
                    LOG_ERROR << "symbol " << currency << "-USD not found for commission calculation." ;
                }
            }
        }
        points = int(fee+0.95);
    }
    else{
        return 0;
    }
    return points;
}
