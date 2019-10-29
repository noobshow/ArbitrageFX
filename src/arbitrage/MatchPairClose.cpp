#include "arbitrage/MatchPairClose.hpp"
#include "common/Config.hpp"

void MatchPairClose::AsyncUpdate(ArbitrageUpdatePtr arbitrageUpdatePtr)
{
    //    LOG_INFO << "ArbitrageManager PriceUpdate: " << tickPtr->timestamp_ << ", "
    //             << tickPtr->broker_ << ", " << tickPtr->symbol_ << ", "
    //             << tickPtr->ask_ << ", " << tickPtr->bid_ ;

//    std::unique_lock<std::mutex> lck (this->openTradeMutex);

//    auto trade_it = this->openTradeMap.find(orderRespCmdPtr->trade_id_);

//    if( trade_it == this->openTradeMap.end() )
//    {
//        LOG_ERROR << "OrderRespCmd not find openTrade: " << orderRespCmdPtr->to_string();
//        this->openTradeMutex.unlock();
//        return;
//    }
//    ArbitrageTradePtr arbTradePtr = trade_it->second;
}

void MatchPairClose::Update(OrderRespCmdPtr orderRespCmdPtr)
{
//    std::unique_lock<std::mutex> lck (this->openTradeMutex);

//    auto trade_it = this->openTradeMap.find(orderRespCmdPtr->trade_id_);

//    if( trade_it == this->openTradeMap.end() )
//    {
//        LOG_ERROR << "OrderRespCmd not find openTrade: " << orderRespCmdPtr->to_string();
//        this->openTradeMutex.unlock();
//        return;
//    }
//    ArbitrageTradePtr arbTradePtr = trade_it->second;

//    OpenOrderInfoPtr orderInfo1;
//    OpenOrderInfoPtr orderInfo2;
//    if( arbTradePtr->openOrderArbedPtr->order_id_ == orderRespCmdPtr->order_id_ )
//    {
//        orderInfo1 = arbTradePtr->arbitratedInfoPtr;
//        orderInfo2 = arbTradePtr->arbitratorInfoPtr;
//    }
//    else if( arbTradePtr->openOrderArborPtr->order_id_ == orderRespCmdPtr->order_id_ )
//    {
//        orderInfo1 = arbTradePtr->arbitratorInfoPtr;
//        orderInfo2 = arbTradePtr->arbitratedInfoPtr;
//    }
//    else{
//        LOG_ERROR << "OrderRespCmd not find order_id_: " << orderRespCmdPtr->to_string();
//        return;
//    }

//    orderInfo1->order_status_ = orderRespCmdPtr->order_status_;
//    if( orderInfo1->order_status_ == ORDER_FILLED )
//    {
//        orderInfo1->opened_price_ = orderRespCmdPtr->price_;
//        LOG_INFO << "Open Order Filled: " << arbTradePtr->arbitratedInfoPtr->to_print();
//    }
//    else if(orderInfo1->order_status_ > ORDER_FILLED){
//        LOG_INFO << "Open Order error: " << arbTradePtr->arbitratedInfoPtr->to_print();
//    }
//    else{
//        LOG_INFO << "Open Order invalid state: " << arbTradePtr->arbitratedInfoPtr->to_print();
//    }

//    if( orderInfo1->order_status_ == ORDER_FILLED && orderInfo2->order_status_ == ORDER_FILLED )
//    {
//        LOG_INFO << "TRADE_OPENED: " << arbTradePtr->trade_id_ << " " << arbTradePtr->symbol_;
//        arbTradePtr->trade_status_ = TRADE_OPENED;
//    }
//    else if( orderInfo1->order_status_ < ORDER_FILLED || orderInfo2->order_status_ < ORDER_FILLED )
//    {
//        LOG_INFO << "Waiting for the second order to be filled. Trade " << arbTradePtr->trade_id_ << " " << arbTradePtr->symbol_;
//    }
//    else if( orderInfo1->order_status_ > ORDER_FILLED && orderInfo2->order_status_ > ORDER_FILLED )
//    {
//        LOG_INFO << "Discarding trade, orders were not filled. Trade " << arbTradePtr->trade_id_ << " " << arbTradePtr->symbol_;
//    }
//    else{
//        LOG_INFO << "TRADE_PARTIALLY_OPENED : " << arbTradePtr->trade_id_ << " " << arbTradePtr->symbol_;
//        arbTradePtr->trade_status_ = TRADE_PARTIALLY_OPENED;
//    }
//    if( arbTradePtr->trade_status_ == TRADE_OPENED || arbTradePtr->trade_status_ == TRADE_PARTIALLY_OPENED )
//    {
//        this->openTradeMap.erase(arbTradePtr->trade_id_);
//        this->tradeMutex.lock();
//        this->tradeMap.insert(std::make_pair(arbTradePtr->trade_id_, arbTradePtr));
//        this->tradeMutex.unlock();
//    }
}

