#include <cstring>
#include <cstdlib>

#include "common/defines.hpp"
#include "ArbitrageCore.hpp"
#include "common/Broker.hpp"
#include "common/loguru.hpp"
#include "arbitrage/ArbitrageReport.hpp"


ArbitrageCore::ArbitrageCore()
{
    this->gainMatchTotal = this->gainNetTotal = 0;
    this->count_min = 0;
    this->timestamp_update = 0;
    this->count_ticks_total = 0;
    this->count_ticks_min = 0;

    int pair_close_sec_mult = Config::getInstance()->pair_close_sec_mult;
    int pair_volume = Config::getInstance()->pair_volume;
    int pair_volume_mult = Config::getInstance()->pair_volume_mult;

    points_open_ = Config::getInstance()->pair_points_open;
    close_points_begin_ = Config::getInstance()->pair_close_points_begin;
    close_points_end_ = Config::getInstance()->pair_close_points_end;
    close_time_step_ = static_cast<uint64_t>(Config::getInstance()->pair_close_sec_step * 1000);
    close_time_mult_ = static_cast<double>(pair_close_sec_mult) / 10;
    volume = static_cast<double>(pair_volume) / 100;
    volume_mult = static_cast<double>(pair_volume_mult) / 10;
    slippage = Config::getInstance()->pair_slippage;
    tradableBrokerSet = Config::getInstance()->pairTradableBrokerSet;
    std::map<int32_t, uint64_t> close_map;

    std::stringstream ss;
    ss << "Close Map Entries: ";
    double e = 0;
    for(int32_t step = close_points_begin_; step <= close_points_end_; step++)
    {
        //uint64_t duration = static_cast<uint64_t>( 60000.0 * ( std::pow( close_time_mult_, e )) );
        double duration_double = 60000.0 * std::pow( close_time_mult_, e );
        uint64_t duration = static_cast<uint64_t>( duration_double );
        this->close_map.insert(std::make_pair( step, duration ));
        e += 1.0;
        ss << "[" << step << ", " << duration << "] ";
    }
    LOG_INFO << ss.str();
}


void ArbitrageCore::UpdatePair(ArbitrageUpdatePtr arbitrageUpdatePtr)
{
    arbitrageUpdatePtr->calculateCenterAndSpread();

    std::string pair_key = this->getPairKey(arbitrageUpdatePtr);

    this->mutex_pair_map.lock();
    ArbitragePairPtr arbPairPtr = getArbitragePair(pair_key);
    this->mutex_pair_map.unlock();

    this->CheckOpenTrade(arbitrageUpdatePtr, arbPairPtr);

    this->CheckCloseTrade(arbitrageUpdatePtr, arbPairPtr);

//#ifdef DEBUG

    this->count_ticks_total++;
    this->count_ticks_min++;
    uint64_t timestamp = arbitrageUpdatePtr->tickArbitratorPtr->timestamp_;
    if( (timestamp - this->timestamp_update) > MINUTE_IN_MS )
    {
        this->CheckTimeout(timestamp);
        count_min++;

        if( (count_min % 5) == 0 ){
            std::unique_lock<std::mutex> lock1(this->mutex_pair_map);
            uint32_t trades_in_pairList = 0;
            for( auto pair_it : arbitragesPairs){
                 trades_in_pairList += pair_it.second->tradesList.size();
            }
            LOG_INFO << "@@@ CORE -> count_min: " << count_min << ", count_ticks_min: " << count_ticks_min
                     << ", count_ticks_total: " << count_ticks_total << ", tradeMap: " << tradeMap.size()
                     << ", arbPairs: " << arbitragesPairs.size() << ", arbTrades: " << trades_in_pairList;
            /*std::stringstream ss;
            for(auto it : this->tradeMap)
                ss << "[" << it.first << ", " << it.second->pair_key_ << "] ";
            LOG_INFO << "@@@ CORE -> TradeMap entries: " << ss.str();*/
            this->count_ticks_min = 0;
        }
        this->timestamp_update = timestamp;
    }
//#endif

}

void ArbitrageCore::CheckOpenTrade(ArbitrageUpdatePtr arbitrageUpdatePtr, ArbitragePairPtr arbPairPtr)
{
    bool openOrder = false;
    bool openArbor = this->tradableBrokerSet.find(arbitrageUpdatePtr->tickArbitratorPtr->broker_) !=  this->tradableBrokerSet.end();
    bool openArbed = this->tradableBrokerSet.find(arbitrageUpdatePtr->tickArbitratedPtr->broker_) !=  this->tradableBrokerSet.end();

    if( !openArbor && !openArbed){
        return;
    }

    if(Broker::refBroker == nullptr){
        LOG_INFO << "Waiting reference broker...";
        return;
    }

    ArbitrageTradePtr arbTradePtr = nullptr;
    int32_t fee1 = 0, fee2 = 0;
    fee1 = Broker::refBroker->getFeeInPoints(arbitrageUpdatePtr->tickArbitratorPtr);
    fee2 = Broker::refBroker->getFeeInPoints(arbitrageUpdatePtr->tickArbitratedPtr);

    int32_t gainMin = this->points_open_;

    this->mutex_pair_map.lock();
    if(arbPairPtr->tradesList.size() > 0){
        gainMin += arbPairPtr->tradesList.back()->arbPointsPtr->gainMatch;
    }
    this->mutex_pair_map.unlock();

    int32_t totalCost = arbitrageUpdatePtr->getMaxArborSpread() +
                        arbitrageUpdatePtr->getMaxArbedSpread() + (fee1 + fee2);

    int32_t minPointsMatch = totalCost + gainMin;

    if(arbitrageUpdatePtr->priceCenterDiff <= -minPointsMatch)
    {
        arbTradePtr = std::make_shared<arbitrage_trade_s>();
        arbTradePtr->symbol_ = arbitrageUpdatePtr->tickArbitratorPtr->symbol_;
        arbTradePtr->pair_key_ = arbPairPtr->key_;
        arbTradePtr->arbitrator_begin_tick = arbitrageUpdatePtr->tickArbitratorPtr;
        arbTradePtr->arbitrated_begin_tick = arbitrageUpdatePtr->tickArbitratedPtr;
        arbTradePtr->arbitrator_side_ = ARBITRATOR_BUY;

        // Create Arbitrator order
        arbTradePtr->openOrderArborPtr = std::make_shared<open_order_cmd_s>(
                    arbTradePtr->arbitrator_begin_tick->broker_,
                    arbTradePtr->trade_id_,
                    arbTradePtr->symbol_,
                    OP_BUY,
                    volume,
                    arbitrageUpdatePtr->tickArbitratorPtr->ask_,
                    slippage);

        // create arbitrated order
        arbTradePtr->openOrderArbedPtr = std::make_shared<open_order_cmd_s>(
                    arbTradePtr->arbitrated_begin_tick->broker_,
                    arbTradePtr->trade_id_,
                    arbTradePtr->symbol_,
                    OP_SELL,
                    volume,
                    arbitrageUpdatePtr->tickArbitratedPtr->bid_,
                    slippage);

        openOrder = true;
    }
    else if(arbitrageUpdatePtr->priceCenterDiff >= minPointsMatch)
    {
        arbTradePtr = std::make_shared<arbitrage_trade_s>();
        arbTradePtr->symbol_ = arbitrageUpdatePtr->tickArbitratorPtr->symbol_;
        arbTradePtr->pair_key_ = arbPairPtr->key_;
        arbTradePtr->arbitrator_begin_tick = arbitrageUpdatePtr->tickArbitratorPtr;
        arbTradePtr->arbitrated_begin_tick = arbitrageUpdatePtr->tickArbitratedPtr;
        arbTradePtr->arbitrator_side_ = ARBITRATOR_SELL;

        // Create Arbitrator order
        arbTradePtr->openOrderArborPtr = std::make_shared<open_order_cmd_s>(
                    arbTradePtr->arbitrator_begin_tick->broker_,
                    arbTradePtr->trade_id_,
                    arbTradePtr->symbol_,
                    OP_SELL,
                    volume,
                    arbitrageUpdatePtr->tickArbitratorPtr->bid_,
                    slippage);

        // create arbitrated order
        arbTradePtr->openOrderArbedPtr = std::make_shared<open_order_cmd_s>(
                    arbTradePtr->arbitrated_begin_tick->broker_,
                    arbTradePtr->trade_id_,
                    arbTradePtr->symbol_,
                    OP_BUY,
                    volume,
                    arbitrageUpdatePtr->tickArbitratedPtr->ask_,
                    slippage);

        openOrder = true;
    }

    if(openOrder)
    {
        if(openArbor){
            arbTradePtr->openOrderArborPtr->order_status_ = ORDER_SEND;
            LOG_INFO << "*** NEW PAIR ORDER : " << arbTradePtr->openOrderArborPtr->to_print();
        }
        else{
            arbTradePtr->openOrderArborPtr->order_status_ = ORDER_DISABLED;
        }
        if(openArbed){
            arbTradePtr->openOrderArbedPtr->order_status_ = ORDER_SEND;
            LOG_INFO << "*** NEW PAIR ORDER : " << arbTradePtr->openOrderArbedPtr->to_print();
        }
        else{
            arbTradePtr->openOrderArbedPtr->order_status_ = ORDER_DISABLED;
        }

        int32_t gainMatch = abs(arbitrageUpdatePtr->priceCenterDiff) - totalCost;
        int32_t totalMatch = abs(arbitrageUpdatePtr->priceCenterDiff);

        arbTradePtr->arbPointsPtr = std::make_shared<arbitrage_points_s>(arbTradePtr->trade_id_, arbPairPtr->key_,
            arbitrageUpdatePtr->priceCenter1, arbitrageUpdatePtr->priceCenter2, arbitrageUpdatePtr->priceCenterDiff,
            arbitrageUpdatePtr->spreadAvgArbitrator, arbitrageUpdatePtr->spreadAvgArbitrated,
            arbitrageUpdatePtr->getMaxArborSpread(), arbitrageUpdatePtr->getMaxArbedSpread(),
            arbitrageUpdatePtr->getExpectedArborSpread(), arbitrageUpdatePtr->getExpectedArbedSpread(),
            0,0, fee1, fee2, gainMin, gainMatch, totalCost, totalMatch);
        arbTradePtr->timestamp_open = arbitrageUpdatePtr->tickArbitratorPtr->timestamp_;

        OpenTrade(arbTradePtr);
    }
}


void ArbitrageCore::CheckCloseTrade(ArbitrageUpdatePtr arbitrageUpdatePtr, ArbitragePairPtr arbPairPtr)
{
    std::unique_lock<std::mutex> lock(this->mutex_pair_map);

    for(ArbitrageTradePtr arbTradePtr : arbPairPtr->tradesList)
    {
        if(arbTradePtr->trade_status_ == TRADE_OPENED || arbTradePtr->trade_status_ == TRADE_PARTIALLY_OPENED)
        {
            bool closeOrder = false;
            int32_t closingPrice1 = 0;
            int32_t closingPrice2 = 0;

            // TODO debugar, verificar se realmente precisa trocar
            if(arbitrageUpdatePtr->tickArbitratorPtr->broker_ == arbTradePtr->openOrderArbedPtr->broker_)
            {
                TickPtr tickSwap = arbitrageUpdatePtr->tickArbitratorPtr;
                arbitrageUpdatePtr->tickArbitratorPtr = arbitrageUpdatePtr->tickArbitratedPtr;
                arbitrageUpdatePtr->tickArbitratedPtr = tickSwap;

                int32_t spreadSwap = arbitrageUpdatePtr->spreadAvgArbitrator;
                arbitrageUpdatePtr->spreadAvgArbitrator = arbitrageUpdatePtr->spreadAvgArbitrated;
                arbitrageUpdatePtr->spreadAvgArbitrated = spreadSwap;

                arbitrageUpdatePtr->calculateCenterAndSpread();
            }

            if(arbitrageUpdatePtr->spread1 <= arbTradePtr->arbPointsPtr->spreadExpected1 &&
               arbitrageUpdatePtr->spread2 <= arbTradePtr->arbPointsPtr->spreadExpected2 )
            {
                uint64_t timestamp = std::max(arbitrageUpdatePtr->tickArbitratorPtr->timestamp_,
                                              arbitrageUpdatePtr->tickArbitratedPtr->timestamp_);
                uint64_t duration = timestamp - arbTradePtr->timestamp_open;

                int32_t close_points = this->getClosePointsByDuration(duration);

                if(arbTradePtr->arbitrator_side_ == ARBITRATOR_BUY)
                {
                    if(arbitrageUpdatePtr->priceCenterDiff >= -close_points)
                    {
                        LOG_INFO << "### Closing BUY SIDE Arbitrage : " << arbTradePtr->to_key_string()
                                 << " PriceDiff: " << arbitrageUpdatePtr->priceCenterDiff;
                        closingPrice1 = arbitrageUpdatePtr->tickArbitratorPtr->bid_;
                        closingPrice2 = arbitrageUpdatePtr->tickArbitratedPtr->ask_;
                        closeOrder = true;
                    }

                }
                else if(arbTradePtr->arbitrator_side_ == ARBITRATOR_SELL)
                {
                    if(arbitrageUpdatePtr->priceCenterDiff <= close_points)
                    {
                        LOG_INFO << "### Closing SELL SIDE Arbitrage : " << arbTradePtr->to_key_string()
                                 << " PriceDiff: " << arbitrageUpdatePtr->priceCenterDiff;
                        closingPrice1 = arbitrageUpdatePtr->tickArbitratorPtr->ask_;
                        closingPrice2 = arbitrageUpdatePtr->tickArbitratedPtr->bid_;
                        closeOrder = true;
                    }
                }
                else{
                    LOG_ERROR << "Arbitrage side undefined. " << arbTradePtr->to_key_string();
                }

                if(closeOrder)
                {
                    uint64_t timestamp_close = 0;
                    arbTradePtr->trade_status_ = TRADE_CLOSING;

                    if(arbTradePtr->openOrderArborPtr->order_status_ == ORDER_FILLED)
                    {
                        CloseOrderCmdPtr closeOrderArborPtr = std::make_shared<close_order_cmd_s>(
                                    arbitrageUpdatePtr->tickArbitratorPtr->broker_,
                                    arbTradePtr->trade_id_,
                                    arbTradePtr->openOrderArborPtr->ticket_,
                                    arbitrageUpdatePtr->tickArbitratorPtr->symbol_,
                                    volume, closingPrice1, slippage );

                        arbTradePtr->closeOrderArborPtr = closeOrderArborPtr;
                        arbTradePtr->arbitrator_close_tick = arbitrageUpdatePtr->tickArbitratorPtr;
                        arbTradePtr->arbPointsPtr->spreadClose1 = arbitrageUpdatePtr->spread1;

                        timestamp_close = arbitrageUpdatePtr->tickArbitratorPtr->timestamp_;

                    }
                    if(arbTradePtr->openOrderArbedPtr->order_status_ == ORDER_FILLED)
                    {
                        CloseOrderCmdPtr closeOrderArbedPtr = std::make_shared<close_order_cmd_s>(
                                    arbitrageUpdatePtr->tickArbitratedPtr->broker_,
                                    arbTradePtr->trade_id_,
                                    arbTradePtr->openOrderArbedPtr->ticket_,
                                    arbitrageUpdatePtr->tickArbitratedPtr->symbol_,
                                    volume, closingPrice2, slippage );

                        arbTradePtr->closeOrderArbedPtr = closeOrderArbedPtr;
                        arbTradePtr->arbitrated_close_tick = arbitrageUpdatePtr->tickArbitratedPtr;
                        arbTradePtr->arbPointsPtr->spreadClose2 = arbitrageUpdatePtr->spread2;

                        if(arbitrageUpdatePtr->tickArbitratedPtr->timestamp_ > timestamp_close)
                            timestamp_close = arbitrageUpdatePtr->tickArbitratedPtr->timestamp_;
                    }
                    arbTradePtr->timestamp_close = timestamp_close;
                    if(arbTradePtr->openOrderArborPtr->order_status_ == ORDER_FILLED)
                        this->CloseOrder(arbTradePtr->closeOrderArborPtr);
                    if(arbTradePtr->openOrderArbedPtr->order_status_ == ORDER_FILLED)
                        this->CloseOrder(arbTradePtr->closeOrderArbedPtr);
                }
            }
        }
    }
}

void ArbitrageCore::Update(OrderRespCmdPtr orderRespCmdPtr)
{
    ArbitrageTradePtr arbTradePtr = this->getTradeById( orderRespCmdPtr->trade_id_ );

    if( arbTradePtr == nullptr )
    {
        LOG_ERROR << "orderRespCmdPtr trade ID " << orderRespCmdPtr->trade_id_ << " not find in TradeMap.";
        return;
    }

    if(orderRespCmdPtr->cmd_type_ == CMD_OPEN_ORDER_RESP)
    {
        this->UpdateOpenOrder(orderRespCmdPtr, arbTradePtr);
    }
    else if(orderRespCmdPtr->cmd_type_ == CMD_CLOSE_ORDER_RESP)
    {
        this->UpdateCloseOrder(orderRespCmdPtr, arbTradePtr);
    }
    else{
        LOG_FATAL << "Order Response with command type invalid: " << orderRespCmdPtr->cmd_type_;
    }
}

void ArbitrageCore::UpdateOpenOrder(OrderRespCmdPtr orderRespCmdPtr, ArbitrageTradePtr arbTradePtr)
{
    if(arbTradePtr->trade_status_ != TRADE_OPENING)
    {
        LOG_ERROR << "Invalid trade status, expected: TRADE_OPENING, current: " << TRADE_STR(arbTradePtr->trade_status_);
        return;
    }

    OpenOrderCmdPtr openOrder1;
    OpenOrderCmdPtr openOrder2;

    if( arbTradePtr->openOrderArbedPtr->order_id_ == orderRespCmdPtr->order_id_ )
    {
        openOrder1 = arbTradePtr->openOrderArbedPtr;
        openOrder2 = arbTradePtr->openOrderArborPtr;
    }
    else if( arbTradePtr->openOrderArborPtr->order_id_ == orderRespCmdPtr->order_id_ )
    {
        openOrder1 = arbTradePtr->openOrderArborPtr;
        openOrder2 = arbTradePtr->openOrderArbedPtr;
    }
    else{
        LOG_FATAL << "OpenOrderResponse with inavalid order id: " << orderRespCmdPtr->to_print();
        return;
    }
    openOrder1->ticket_ = orderRespCmdPtr->ticket_;

    //std::unique_lock<std::mutex> lock(this->mutex_trade_status);
    switch (orderRespCmdPtr->order_status_)
    {
    case ORDER_CREATED:
    case ORDER_SEND:
    {
        LOG_FATAL << "OpenOrderResponse with Invalid order status: " << orderRespCmdPtr->to_print();
        openOrder1->order_status_ = ORDER_ERROR;
        if(openOrder2->order_status_ > ORDER_FILLED)
        {
            arbTradePtr->trade_status_ = TRADE_OPEN_ERROR;
            LOG_ERROR << "TRADE_OPEN_ERROR: " << arbTradePtr->to_key_string();
            this->RemoveTradeMap(arbTradePtr);
        }
        break;
    }
    case ORDER_FILLED:
    {
        openOrder1->order_status_ = ORDER_FILLED;
        openOrder1->opened_price_ = orderRespCmdPtr->price_;
        LOG_INFO << "OPEN_ORDER_FILLED: " << openOrder1->to_print();

        if(openOrder2->order_status_ == ORDER_FILLED) {
            arbTradePtr->trade_status_ = TRADE_OPENED;
            LOG_INFO << "TRADE_OPENED: " << arbTradePtr->to_key_string();
        }
        else if( openOrder2->order_status_ > ORDER_FILLED ){
            arbTradePtr->trade_status_ = TRADE_PARTIALLY_OPENED;
            LOG_INFO << "TRADE_PARTIALLY_OPENED: " << arbTradePtr->to_key_string();
        }
        break;
    }
    case ORDER_ERROR:
    case ORDER_TIMEOUT:
    case ORDER_DISABLED:
    {
        openOrder1->order_status_ = orderRespCmdPtr->order_status_;
        LOG_ERROR << "OPEN_ORDER_ERROR: " << openOrder1->to_print();

        if(openOrder2->order_status_ == ORDER_FILLED) {
            arbTradePtr->trade_status_ = TRADE_PARTIALLY_OPENED;
            LOG_INFO << "TRADE_PARTIALLY_OPENED: " << arbTradePtr->to_key_string();
        }
        else if( openOrder2->order_status_ > ORDER_FILLED ){
            arbTradePtr->trade_status_ = TRADE_OPEN_ERROR;
            LOG_ERROR << "TRADE_OPEN_ERROR: " << arbTradePtr->to_key_string();
            this->RemoveTradeMap(arbTradePtr);
        }
        break;
    }
    }
}

void ArbitrageCore::UpdateCloseOrder(OrderRespCmdPtr orderRespCmdPtr, ArbitrageTradePtr arbTradePtr)
{
    CloseOrderCmdPtr closeOrder1;
    CloseOrderCmdPtr closeOrder2;
    OpenOrderCmdPtr openOrder1;
    OpenOrderCmdPtr openOrder2;

    if( arbTradePtr->closeOrderArbedPtr &&
        arbTradePtr->closeOrderArbedPtr->order_id_ == orderRespCmdPtr->order_id_ )
    {
        openOrder1 = arbTradePtr->openOrderArbedPtr;
        openOrder2 = arbTradePtr->openOrderArborPtr;
        closeOrder1 = arbTradePtr->closeOrderArbedPtr;
        closeOrder2 = arbTradePtr->closeOrderArborPtr;
    }
    else if( arbTradePtr->closeOrderArborPtr &&
             arbTradePtr->closeOrderArborPtr->order_id_ == orderRespCmdPtr->order_id_ )
    {
        openOrder1 = arbTradePtr->openOrderArborPtr;
        openOrder2 = arbTradePtr->openOrderArbedPtr;
        closeOrder1 = arbTradePtr->closeOrderArborPtr;
        closeOrder2 = arbTradePtr->closeOrderArbedPtr;
    }
    else{
        LOG_ERROR << "CloseOrderResponse with invalid order id: " << orderRespCmdPtr->to_print();
        return;
    }

    //std::unique_lock<std::mutex> lock(this->mutex_trade_status);
    switch (orderRespCmdPtr->order_status_)
    {
    case ORDER_CREATED:
    case ORDER_SEND:
    {
        LOG_FATAL << "CloseOrderResponse with Invalid order status: " << orderRespCmdPtr->to_print();
        closeOrder1->order_status_ = ORDER_ERROR;
        if( closeOrder2 == nullptr || closeOrder2->order_status_ >= ORDER_FILLED){
            this->TryCloseOrder( arbTradePtr );
        }
        break;
    }
    case ORDER_FILLED:
    {
        closeOrder1->order_status_ = ORDER_FILLED;
        closeOrder1->closed_price_ = orderRespCmdPtr->price_;

        if(openOrder1->order_type_ == OP_BUY){
            closeOrder1->profit_points_ = closeOrder1->closed_price_ - openOrder1->opened_price_;
        }
        else if(openOrder1->order_type_ == OP_SELL){
            closeOrder1->profit_points_ = openOrder1->opened_price_ - closeOrder1->closed_price_;
        }
        else{
            LOG_FATAL << "CloseOrderResponse found invalid order type: " << openOrder1->order_type_;
        }
        closeOrder1->profit_usd_ = static_cast<double>(closeOrder1->profit_points_) *
                                   Broker::refBroker->getPointUsdValue(arbTradePtr->symbol_);

        arbTradePtr->profit_points += closeOrder1->profit_points_;
        arbTradePtr->profit_usd += closeOrder1->profit_usd_;

        LOG_INFO << "CLOSE_ORDER_FILLED: " << closeOrder1->to_print();

        if(closeOrder2 == nullptr || closeOrder2->order_status_ == ORDER_FILLED)
        {
            arbTradePtr->trade_status_ = TRADE_CLOSED;
            LOG_INFO << "TRADE_CLOSED: " << arbTradePtr->to_key_string();
            this->RemoveTradeMap(arbTradePtr);
            ArbitrageReport::getInstance()->addData(arbTradePtr);
        }
        else if(closeOrder2->order_status_ > ORDER_FILLED ){
            this->TryCloseOrder( arbTradePtr );
        }
        break;
    }
    case ORDER_ERROR:
    case ORDER_TIMEOUT:
    case ORDER_DISABLED:
    {
        closeOrder1->order_status_ = orderRespCmdPtr->order_status_;
        LOG_ERROR << "ORDER_ERROR: " << closeOrder1->to_print();

        if(closeOrder2 == nullptr || closeOrder2->order_status_ >= ORDER_FILLED) {
            this->TryCloseOrder(arbTradePtr);
        }
        break;
    }
    }
}

void ArbitrageCore::TryCloseOrder(ArbitrageTradePtr arbTradePtr)
{
    LOG_ERROR << "TRADE_TRYING_CLOSE: " << arbTradePtr->to_key_string();

    int32_t orders = 0;
    if( arbTradePtr->openOrderArborPtr->order_status_ == ORDER_FILLED &&
        arbTradePtr->closeOrderArborPtr->order_status_ != ORDER_FILLED )
    {
        arbTradePtr->openOrderArborPtr->try_close_++;
        LOG_INFO << "TRYING CLOSE ORDER (n=" << arbTradePtr->openOrderArborPtr->try_close_
                 << ", Order: " << arbTradePtr->openOrderArborPtr->to_print();
        orders++;
    }
    if( arbTradePtr->openOrderArbedPtr->order_status_ == ORDER_FILLED &&
        arbTradePtr->closeOrderArbedPtr->order_status_ != ORDER_FILLED )
    {
        arbTradePtr->openOrderArbedPtr->try_close_++;
        LOG_INFO << "TRYING CLOSE ORDER (n=" << arbTradePtr->openOrderArbedPtr->try_close_
                 << ", Order: " << arbTradePtr->openOrderArbedPtr->to_print();
        orders++;
    }
    if( orders > 1 )
        arbTradePtr->trade_status_ = TRADE_OPENED;
    else if( orders == 1)
        arbTradePtr->trade_status_ = TRADE_PARTIALLY_OPENED;
    else{
        LOG_FATAL << "TryCloseOrder ERROR, no open order to attempt to close. " << arbTradePtr->to_key_string();
        arbTradePtr->trade_status_ = TRADE_CLOSE_ERROR;
        this->RemoveTradeMap(arbTradePtr);
    }
}


void ArbitrageCore::CheckTimeout(uint64_t timestamp)
{
    std::vector<ArbitrageTradePtr> removeList;
    //std::unique_lock<std::mutex> lock(this->mutex_trade_status);
    this->mutex_trade_map.lock();

    for ( auto it_trade : this->tradeMap )
    {
        if(it_trade.second->trade_status_ == TRADE_OPENING)
        {
            if( it_trade.second->openOrderArborPtr->order_status_ == ORDER_SEND &&
                (timestamp - it_trade.second->arbitrator_begin_tick->timestamp_ ) > MINUTE_IN_MS )
            {
                LOG_ERROR << "Order TIMEOUT: " << it_trade.second->openOrderArborPtr->to_print();
                it_trade.second->openOrderArborPtr->order_status_ = ORDER_TIMEOUT;

                if(it_trade.second->openOrderArbedPtr->order_status_ == ORDER_FILLED) {
                    it_trade.second->trade_status_ = TRADE_PARTIALLY_OPENED;
                    LOG_INFO << "TRADE_PARTIALLY_OPENED: " << it_trade.second->to_key_string();
                }
                else if( it_trade.second->openOrderArbedPtr->order_status_ > ORDER_FILLED ){
                    it_trade.second->trade_status_ = TRADE_OPEN_ERROR;
                    LOG_ERROR << "TRADE_OPEN_ERROR: " << it_trade.second->to_key_string();
                    removeList.push_back(it_trade.second);
                }
            }
            if( it_trade.second->openOrderArbedPtr->order_status_ == ORDER_SEND &&
                (timestamp - it_trade.second->arbitrated_begin_tick->timestamp_ ) > MINUTE_IN_MS )
            {
                LOG_ERROR << "Order TIMEOUT: " << it_trade.second->openOrderArbedPtr->to_print();
                it_trade.second->openOrderArbedPtr->order_status_ = ORDER_TIMEOUT;

                if(it_trade.second->openOrderArborPtr->order_status_ == ORDER_FILLED) {
                    it_trade.second->trade_status_ = TRADE_PARTIALLY_OPENED;
                    LOG_INFO << "TRADE_PARTIALLY_OPENED: " << it_trade.second->to_key_string();
                }
                else if( it_trade.second->openOrderArborPtr->order_status_ > ORDER_FILLED ){
                    it_trade.second->trade_status_ = TRADE_OPEN_ERROR;
                    LOG_ERROR << "TRADE_OPEN_ERROR: " << it_trade.second->to_key_string();
                    removeList.push_back(it_trade.second);
                }
            }
        }
    }
    this->mutex_trade_map.unlock();

    for(auto trade : removeList){
        this->RemoveTradeMap(trade);
    }
}

ArbitragePairPtr ArbitrageCore::getArbitragePair(std::string key)
{
    ArbitragePairPtr arbPairPtr;
    auto it = this->arbitragesPairs.find(key);
    if(it == this->arbitragesPairs.end())
    {
        arbPairPtr = std::make_shared<arbitrage_pair_s>(key);
        this->arbitragesPairs.insert(std::make_pair(key, arbPairPtr));
    }
    else{
        arbPairPtr = it->second;
    }
    return arbPairPtr;
}

std::string ArbitrageCore::getPairKey(ArbitrageUpdatePtr arbitrageUpdatePtr)
{
    std::string key = arbitrageUpdatePtr->tickArbitratorPtr->symbol_;
    if( arbitrageUpdatePtr->tickArbitratorPtr->broker_ < arbitrageUpdatePtr->tickArbitratedPtr->broker_){
        key = key + arbitrageUpdatePtr->tickArbitratorPtr->broker_ + arbitrageUpdatePtr->tickArbitratedPtr->broker_;
    }
    else{
        key = key + arbitrageUpdatePtr->tickArbitratedPtr->broker_ + arbitrageUpdatePtr->tickArbitratorPtr->broker_;
    }
    return key;
}

void ArbitrageCore::OpenTrade(ArbitrageTradePtr arbTradePtr)
{
    arbTradePtr->trade_status_ = TRADE_OPENING;
    this->InsertTradeMap(arbTradePtr);
    if(arbTradePtr->openOrderArborPtr->order_status_ == ORDER_SEND)
        this->mt4PublisherPtr->AsyncUpdate(arbTradePtr->openOrderArborPtr);
    if(arbTradePtr->openOrderArbedPtr->order_status_ == ORDER_SEND)
        this->mt4PublisherPtr->AsyncUpdate(arbTradePtr->openOrderArbedPtr);
}

void ArbitrageCore::CloseTrade(ArbitrageTradePtr arbTradePtr)
{
    this->mt4PublisherPtr->AsyncUpdate(arbTradePtr->closeOrderArborPtr);
    this->mt4PublisherPtr->AsyncUpdate(arbTradePtr->closeOrderArbedPtr);
}

void ArbitrageCore::CloseOrder(CloseOrderCmdPtr closeOrderPtr)
{
    closeOrderPtr->order_status_ = ORDER_SEND;
    this->mt4PublisherPtr->AsyncUpdate(closeOrderPtr);
}

void ArbitrageCore::InsertTradeMap(ArbitrageTradePtr arbTradePtr)
{
    this->mutex_pair_map.lock();
    ArbitragePairPtr arbPairPtr = getArbitragePair(arbTradePtr->pair_key_);
    arbPairPtr->tradesList.push_back(arbTradePtr);
    this->mutex_pair_map.unlock();

    this->mutex_trade_map.lock();
    this->tradeMap.insert(std::make_pair(arbTradePtr->trade_id_, arbTradePtr));
    this->mutex_trade_map.unlock();
}

void ArbitrageCore::RemoveTradeMap(ArbitrageTradePtr arbTradePtr)
{
    this->mutex_pair_map.lock();
    ArbitragePairPtr arbPairPtr = getArbitragePair(arbTradePtr->pair_key_);
    auto it = arbPairPtr->tradesList.begin();
    for ( ; it != arbPairPtr->tradesList.end(); ) {
      if ((*it)->trade_id_ == arbTradePtr->trade_id_ ) {
        it = arbPairPtr->tradesList.erase(it);
      } else {
        ++it;
      }
    }
    this->mutex_pair_map.unlock();

    this->mutex_trade_map.lock();
    this->tradeMap.erase(arbTradePtr->trade_id_);
    this->mutex_trade_map.unlock();
}


ArbitrageTradePtr ArbitrageCore::getTradeById(int32_t id)
{
    ArbitrageTradePtr arbTradePtr = nullptr;
    this->mutex_trade_map.lock();
    auto trade_it = this->tradeMap.find(id);
    if( trade_it != this->tradeMap.end() )
    {
        arbTradePtr = trade_it->second;
    }
    this->mutex_trade_map.unlock();
    return arbTradePtr;
}

int32_t ArbitrageCore::getClosePointsByDuration(uint64_t duration)
{
    int32_t points = this->close_points_end_*2;
    for( auto it : this->close_map )
    {
        if(duration <= it.second)
            return it.first;
    }
    return points;
}
