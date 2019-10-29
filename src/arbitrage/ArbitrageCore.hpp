#ifndef MATCHPAIROPEN_H
#define MATCHPAIROPEN_H

#include <unordered_map>
#include <atomic>
#include <set>

#include "common/Dispatcher.hpp"
#include "common/Observer.hpp"
#include "common/Types.hpp"
#include "iostream/Mt4Publisher.hpp"

const static uint64_t MINUTE_IN_MS = 60000;

typedef std::map<int32_t, ArbitrageTradePtr> TradeMap;

class ArbitrageCore :
        public Dispatcher<std::string>,
        public Observer<order_resp_cmd_s>
{
private:
    int32_t points_open_;
    int32_t close_points_begin_;
    int32_t close_points_end_;
    uint64_t close_time_step_;
    double close_time_mult_;
    double volume;
    double volume_mult;
    int32_t slippage;
    std::map<int32_t, uint64_t> close_map;
    std::set<std::string> tradableBrokerSet;

    int32_t gainMatchTotal;
    int32_t gainNetTotal;
    atomic<uint64_t> count_min;
    atomic<uint64_t> count_ticks_total;
    atomic<uint64_t> count_ticks_min;
    atomic<uint64_t> timestamp_update;

    // trade map safe
    TradeMap tradeMap;
    std::mutex mutex_trade_map;

    // arbitrage pairs safe
    std::unordered_map<std::string, ArbitragePairPtr> arbitragesPairs;
    std::mutex mutex_pair_map;

    std::mutex mutex_trade_status;


    Mt4PublisherPtr mt4PublisherPtr;

    std::string getPairKey(ArbitrageUpdatePtr arbitrageUpdatePtr);
    ArbitragePairPtr getArbitragePair(std::string key);
    ArbitrageTradePtr getTradeById(int32_t id);
    int32_t getClosePointsByDuration(uint64_t duration);

    void OpenTrade(ArbitrageTradePtr arbTradePtr);
    void CloseTrade(ArbitrageTradePtr arbTradePtr);
    void CloseOrder(CloseOrderCmdPtr closeOrderPtr);
    void InsertTradeMap(ArbitrageTradePtr arbTradePtr);
    void RemoveTradeMap(ArbitrageTradePtr arbTradePtr);

    void CheckOpenTrade(ArbitrageUpdatePtr arbitrageUpdatePtr, ArbitragePairPtr arbPairPtr);
    void CheckCloseTrade(ArbitrageUpdatePtr arbitrageUpdatePtr, ArbitragePairPtr arbPairPtr);
    void UpdateOpenOrder(OrderRespCmdPtr orderRespCmdPtr, ArbitrageTradePtr arbTradePtr);
    void UpdateCloseOrder(OrderRespCmdPtr orderRespCmdPtr, ArbitrageTradePtr arbTradePtr);
    void TryCloseOrder(ArbitrageTradePtr arbTradePtr);
    void CheckTimeout(uint64_t timestamp);
protected:
public:
    ArbitrageCore();
    void UpdatePair(ArbitrageUpdatePtr arbitrageUpdatePtr);
    void Update(OrderRespCmdPtr orderRespCmdPtr);
    void AttachMt4Publisher(Mt4PublisherPtr mt4PubPtr) {
        this->mt4PublisherPtr = mt4PubPtr;
    }
};

typedef std::shared_ptr<ArbitrageCore> ArbitrageCorePtr;

#endif // MATCHPAIROPEN_H
