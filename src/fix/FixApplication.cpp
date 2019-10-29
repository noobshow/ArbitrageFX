/* -*- C++ -*- */

/****************************************************************************
** Copyright (c) 2001-2014
**
** This file is part of the QuickFIX FIX Engine
**
** This file may be distributed under the terms of the quickfixengine.org
** license as defined by quickfixengine.org and appearing in the file
** LICENSE included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.quickfixengine.org/LICENSE for licensing information.
**
** Contact ask@quickfixengine.org if any conditions of this licensing are
** not clear to you.
**
****************************************************************************/

#include "fix/FixApplication.hpp"
#include "common/loguru.hpp"

namespace FIX
{
namespace FIELD
{
const int MinInc = 6350;
const int MinBR = 6351;
const int YTM = 6360;
const int YTW = 6361;
}
DEFINE_QTY(MinInc);
DEFINE_QTY(MinBR);
DEFINE_PERCENTAGE(YTM);
DEFINE_PERCENTAGE(YTW);
}
namespace FIX44
{
class NoMDEntriesBondsPro : public MarketDataIncrementalRefresh::NoMDEntries//add custom fields to the MarketDataIncrementalRefresh message
{
public:
    NoMDEntriesBondsPro() : MarketDataIncrementalRefresh::NoMDEntries() {}
    FIELD_SET(*this, FIX::MinInc)
    FIELD_SET(*this, FIX::MinBR)
    FIELD_SET(*this, FIX::YTM)
    FIELD_SET(*this, FIX::YTW)
};
}

FixApplication::FixApplication(FIX::BeginString beginStr, FIX::SenderCompID senderCID, FIX::TargetCompID targetCID){
    this->beginString = beginStr;
    this->senderCompID = senderCID;
    this->targetCompID = targetCID;
    header.setField(senderCID);
    header.setField(targetCompID);

    if(beginStr == "FIX.4.4")
        this->version = 44;
    else if(beginStr == "FIX.5.0")
        this->version = 50;
}

void FixApplication::onLogon( const FIX::SessionID& sessionID )
{
    LOG_INFO << std::endl << "FixApplication::onLogon - " << sessionID;
}

void FixApplication::onLogout( const FIX::SessionID& sessionID )
{
    LOG_INFO << std::endl << "FixApplication::onLogout - " << sessionID;
}

void FixApplication::toAdmin( FIX::Message& message, const FIX::SessionID& )
{
    if (FIX::MsgType_Logon == message.getHeader().getField(FIX::FIELD::MsgType))
    {
        message.setField(FIX::Username("1020650"));
        message.setField(FIX::Password("ZOBL8661"));
        //message.setField(FIX::TargetSubID("TRADE"));
        //message.setField(FIX::ProtocolSpec("ext.1.21"));
    }
    LOG_INFO << "FixApplication::toAdmin - OUT: " << message;
}

void FixApplication::fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType )
{
    crack( message, sessionID );
    LOG_INFO << "FixApplication::fromApp - IN: " << message;
}

void FixApplication::toApp( FIX::Message& message, const FIX::SessionID& sessionID )
throw( FIX::DoNotSend )
{
    try
    {
        FIX::PossDupFlag possDupFlag;
        message.getHeader().getField( possDupFlag );
        if ( possDupFlag ) throw FIX::DoNotSend();
    }
    catch ( FIX::FieldNotFound& ) {}

    LOG_INFO << "FixApplication::toApp - OUT: " << message;
}

void FixApplication::onMessage( const FIX44::ExecutionReport&, const FIX::SessionID& )
{
    LOG_INFO << "Begin FixApplication::onMessage( FIX44::ExecutionReport&, FIX::SessionID& )";
}

void FixApplication::onMessage( const FIX44::OrderCancelReject&, const FIX::SessionID& )
{
    LOG_INFO << "Begin FixApplication::onMessage( FIX44::OrderCancelReject&, FIX::SessionID& )";
}

void FixApplication::onMessage
( const FIX44::MarketDataIncrementalRefresh& message, const FIX::SessionID& )
{
    LOG_INFO << "Application::onMessage( const FIX44::MarketDataIncrementalRefresh& , FIX::SessionID& )";
    FIX::NoMDEntries noMDEntries;
    message.get(noMDEntries);
    if (noMDEntries.getValue()!=1){
        LOG_INFO << "NoMDEntries in MarketDataIncrementalRefresh is not 1!";
        return;
    }
    FIX44::MarketDataIncrementalRefresh::NoMDEntries group;
    message.getGroup(1, group);

    FIX::MDEntryID entryID; group.get(entryID);
    FIX::MDUpdateAction action; group.get(action);
    char actionvalue = action.getValue();//0=New, 1=Update, 2=Delete)
    if (actionvalue=='2') //ignore the delete
    {
        std::map<std::string, SECURITY>::iterator it = securities_.end();
        it=securities_.find(entryID);
        if (it!=securities_.end())
            securities_.erase(it);
        return;
    }
    SECURITY security;
    security.MDEntryID = entryID;
    security.MDUpdateAction = action;
    FIX::Symbol symbol;
    if(group.isSet(symbol)){
        group.get(symbol);
        security.Symbol = symbol;
    }
    FIX::MDEntryType entryType;
    if(group.isSet(entryType)) {
        group.get(entryType);
        security.MDEntryType = entryType;
    }
    FIX::MDEntryPx price;
    if(group.isSet(price)) {
        group.get(price);
        security.MDEntryPx		= price.getValue();
    }
    FIX::MDEntrySize size;
    if(group.isSet(size)) {
        group.get(size);
        security.MDEntrySize	= size.getValue();
    }
    FIX::MinQty qty;
    if(group.isSet(qty)) {
        group.get(qty);
        security.MinQty			= qty.getValue();
    }
    FIX::MinInc inc;
    if(message.isSetField(inc)) {
        message.getField(inc);
        security.MinInc			= inc.getValue();
    }
    FIX::MinBR br;
    if(message.isSetField(br)) {
        message.getField(br);
        security.MinBR			= br.getValue();
    }
    FIX::YTM ytm;
    if(message.isSetField(ytm)) {
        message.getField(ytm);
        security.YTM			= ytm.getValue();
    }
    FIX::YTW ytw;
    if(message.isSetField(ytw)) {
        message.getField(ytw);
        security.YTW			= ytw.getValue();
    }
    securities_[entryID] = security;
}

void FixApplication::onMessage( const FIX50::ExecutionReport&, const FIX::SessionID& )
{
    LOG_INFO << "Begin FixApplication::onMessage( FIX50::ExecutionReport&, FIX::SessionID& )";
}

void FixApplication::onMessage( const FIX50::OrderCancelReject&, const FIX::SessionID& )
{
    LOG_INFO << "Begin FixApplication::onMessage( FIX50::OrderCancelReject&, FIX::SessionID& )";
}


void FixApplication::EnterOrder(
        std::string id, std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side, FIX::TimeInForce timeInForce)
{
    LOG_INFO << "Begin FixApplication::EnterOrder";
    FIX::Message order;

    switch ( this->version ) {
    case 44: order = NewOrderSingle44(id, symbol, price, orderQty, ordType, side, timeInForce); break;
    case 50: order = NewOrderSingle50(id, symbol, price, orderQty, ordType, side, timeInForce); break;
    default: LOG_ERROR << "FixApplication::EnterOrder: No test for version " << version; break;
    }

    FIX::Session::sendToTarget( order );
}

void FixApplication::CancelOrder(std::string origId, std::string id, std::string symbol, double orderQty, FIX::Side side)
{
    LOG_INFO << "Begin FixApplication::CancelOrder";
    FIX::Message cancel;

    switch ( this->version ) {
    case 44: cancel = OrderCancelRequest44(origId, id, symbol, orderQty, side); break;
    case 50: cancel = OrderCancelRequest50(origId, id, symbol, orderQty, side); break;
    default: LOG_ERROR << "FixApplication::queryCancelOrder: No test for version " << version; break;
    }
    FIX::Session::sendToTarget( cancel );
}

void FixApplication::ReplaceOrder(std::string origId, std::string id, std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side)
{
    LOG_INFO << "Begin FixApplication::ReplaceOrder";
    FIX::Message replace;

    switch ( this->version ) {
    case 44: replace = CancelReplaceRequest44(origId, id, symbol, price, orderQty, ordType, side); break;
    case 50: replace = CancelReplaceRequest50(origId, id, symbol, price, orderQty, ordType, side); break;
    default: LOG_ERROR << "FixApplication::ReplaceOrder: No test for version " << version; break;
    }
    FIX::Session::sendToTarget( replace );
}

void FixApplication::MarketDataRequest()
{
    LOG_INFO << "Begin FixApplication::MarketDataRequest";
    FIX::Message md;

    switch (this->version) {
    case 44: md = MarketDataRequest44(); break;
    case 50: md = MarketDataRequest50(); break;
    default: std::cerr << "FixApplication::MarketDataRequest: No test for version " << version << std::endl; break;
    }
    FIX::Session::sendToTarget( md );
}

FIX44::NewOrderSingle FixApplication::NewOrderSingle44(
        std::string id, std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side, FIX::TimeInForce timeInForce)
{
    FIX44::NewOrderSingle newOrderSingle( FIX::ClOrdID(id), side, FIX::TransactTime(), ordType );

    newOrderSingle.set( FIX::HandlInst('1') );
    newOrderSingle.set( FIX::Symbol(symbol) );
    newOrderSingle.set( FIX::OrderQty(orderQty) );
    newOrderSingle.set( timeInForce );
    if ( ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT )
        newOrderSingle.set( FIX::Price(price) );
    if ( ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT )
        newOrderSingle.set( FIX::StopPx(price) );

    newOrderSingle.getHeader().setField(this->senderCompID);
    newOrderSingle.getHeader().setField(this->targetCompID);

    return newOrderSingle;
}

FIX50::NewOrderSingle FixApplication::NewOrderSingle50(
        std::string id, std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side, FIX::TimeInForce timeInForce)
{
    FIX50::NewOrderSingle newOrderSingle(FIX::ClOrdID(id), side, FIX::TransactTime(), ordType );

    newOrderSingle.set( FIX::HandlInst('1') );
    newOrderSingle.set( FIX::Symbol(symbol) );
    newOrderSingle.set( FIX::OrderQty(orderQty) );
    newOrderSingle.set( timeInForce );
    if ( ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_STOP_LIMIT )
        newOrderSingle.set( FIX::Price(price) );
    if ( ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT )
        newOrderSingle.set( FIX::StopPx(price) );
    newOrderSingle.getHeader().setField(this->senderCompID);
    newOrderSingle.getHeader().setField(this->targetCompID);
    return newOrderSingle;
}

FIX44::OrderCancelRequest FixApplication::OrderCancelRequest44(
        std::string origId, std::string id, std::string symbol, double orderQty, FIX::Side side)
{
    FIX44::OrderCancelRequest orderCancelRequest(
                FIX::OrigClOrdID(origId), FIX::ClOrdID(id), side, FIX::TransactTime() );

    orderCancelRequest.set( FIX::Symbol(symbol) );
    orderCancelRequest.set( FIX::OrderQty(orderQty) );
    orderCancelRequest.getHeader().setField(this->senderCompID);
    orderCancelRequest.getHeader().setField(this->targetCompID);
    return orderCancelRequest;
}

FIX50::OrderCancelRequest FixApplication::OrderCancelRequest50(
        std::string origId, std::string id, std::string symbol, double orderQty, FIX::Side side)
{
    FIX50::OrderCancelRequest orderCancelRequest(
                FIX::OrigClOrdID(origId), FIX::ClOrdID(id), side, FIX::TransactTime() );

    orderCancelRequest.set( FIX::Symbol(symbol) );
    orderCancelRequest.set( FIX::OrderQty(orderQty) );
    orderCancelRequest.getHeader().setField(this->senderCompID);
    orderCancelRequest.getHeader().setField(this->targetCompID);
    return orderCancelRequest;
}

FIX44::OrderCancelReplaceRequest FixApplication::CancelReplaceRequest44(
        std::string origId, std::string id, std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side)
{
    FIX44::OrderCancelReplaceRequest cancelReplaceRequest(
                FIX::OrigClOrdID(origId), FIX::ClOrdID(id), side, FIX::TransactTime(), ordType );

    cancelReplaceRequest.set( FIX::HandlInst('1') );
    cancelReplaceRequest.set( FIX::Symbol(symbol) );
    cancelReplaceRequest.set( FIX::Price(price) );
    cancelReplaceRequest.set( FIX::OrderQty(orderQty) );
    cancelReplaceRequest.getHeader().setField(this->senderCompID);
    cancelReplaceRequest.getHeader().setField(this->targetCompID);
    return cancelReplaceRequest;
}

FIX50::OrderCancelReplaceRequest FixApplication::CancelReplaceRequest50(
        std::string origId, std::string id, std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side)
{
    FIX50::OrderCancelReplaceRequest cancelReplaceRequest(
                FIX::OrigClOrdID(origId), FIX::ClOrdID(id), side, FIX::TransactTime(), ordType );

    cancelReplaceRequest.set( FIX::HandlInst('1') );
    cancelReplaceRequest.set( FIX::Symbol(symbol) );
    cancelReplaceRequest.set( FIX::Price(price) );
    cancelReplaceRequest.set( FIX::OrderQty(orderQty) );
    cancelReplaceRequest.getHeader().setField(this->senderCompID);
    cancelReplaceRequest.getHeader().setField(this->targetCompID);
    return cancelReplaceRequest;
}

FIX44::MarketDataRequest FixApplication::MarketDataRequest44()
{
    FIX::MDReqID mdReqID( "MARKETDATAID" );
    FIX::SubscriptionRequestType subType( FIX::SubscriptionRequestType_SNAPSHOT );
    FIX::MarketDepth marketDepth( 0 );

    FIX44::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
    FIX::MDEntryType mdEntryType( FIX::MDEntryType_BID );
    marketDataEntryGroup.set( mdEntryType );

    FIX44::MarketDataRequest::NoRelatedSym symbolGroup;
    FIX::Symbol symbol( "EURUSD" );
    symbolGroup.set( symbol );

    FIX44::MarketDataRequest message( mdReqID, subType, marketDepth );
    message.addGroup( marketDataEntryGroup );
    message.addGroup( symbolGroup );

    message.getHeader().setField(this->senderCompID);
    message.getHeader().setField(this->targetCompID);

    LOG_INFO << message.toXML();
    LOG_INFO << message.toString();

    return message;
}

FIX50::MarketDataRequest FixApplication::MarketDataRequest50()
{
    FIX::MDReqID mdReqID( "MARKETDATAID" );
    FIX::SubscriptionRequestType subType( FIX::SubscriptionRequestType_SNAPSHOT );
    FIX::MarketDepth marketDepth( 0 );

    FIX50::MarketDataRequest::NoMDEntryTypes marketDataEntryGroup;
    FIX::MDEntryType mdEntryType( FIX::MDEntryType_BID );
    marketDataEntryGroup.set( mdEntryType );

    FIX50::MarketDataRequest::NoRelatedSym symbolGroup;
    FIX::Symbol symbol( "LNUX" );
    symbolGroup.set( symbol );

    FIX50::MarketDataRequest message( mdReqID, subType, marketDepth );
    message.addGroup( marketDataEntryGroup );
    message.addGroup( symbolGroup );

    message.getHeader().setField(this->senderCompID);
    message.getHeader().setField(this->targetCompID);

    LOG_INFO << message.toXML();
    LOG_INFO << message.toString();

    return message;
}
