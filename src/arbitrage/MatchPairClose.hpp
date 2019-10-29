#ifndef MATCHPAIRCLOSE_H
#define MATCHPAIRCLOSE_H

#include <map>
#include <mutex>

#include "common/Dispatcher.hpp"
#include "common/Observer.hpp"
#include "common/Types.hpp"

typedef std::map<int32_t, ArbitrageTradePtr> TradeMap;

class MatchPairClose :
        public Dispatcher<open_order_cmd_s>,
        public Observer<order_resp_cmd_s>,
        public AsyncObserver<arbitrage_update_s>
{
private:
    std::mutex openTradeMutex;
    std::mutex tradeMutex;
    std::mutex closeTradeMutex;
    TradeMap openTradeMap;
    TradeMap tradeMap;
    TradeMap closeTradeMap;

public:
    MatchPairClose(){}
    ~MatchPairClose(){}

    void Update(ArbitrageTradePtr arbTradePtr);
    void Update(OrderRespCmdPtr orderRespCmdPtr);
    void AsyncUpdate(ArbitrageUpdatePtr arbitrageUpdatePtr);
};

typedef std::shared_ptr<MatchPairClose> MatchPairClosePtr;

#endif // MATCHPAIRCLOSE_H
