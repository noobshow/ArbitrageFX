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

#ifndef FIXAPPLICATION_H
#define FIXAPPLICATION_H

#pragma GCC diagnostic ignored "-Wdeprecated"

#include <iostream>
#include "common/Utils.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/Values.h>
#include <quickfix/Mutex.h>
#include <quickfix/Session.h>
#include <quickfix/FieldMap.h>

#include <quickfix/fix44/NewOrderSingle.h>
#include <quickfix/fix44/ExecutionReport.h>
#include <quickfix/fix44/OrderCancelRequest.h>
#include <quickfix/fix44/OrderCancelReject.h>
#include <quickfix/fix44/OrderCancelReplaceRequest.h>
#include <quickfix/fix44/MarketDataRequest.h>
#include <quickfix/fix44/MarketDataIncrementalRefresh.h>

#include <quickfix/fix50/NewOrderSingle.h>
#include <quickfix/fix50/ExecutionReport.h>
#include <quickfix/fix50/OrderCancelRequest.h>
#include <quickfix/fix50/OrderCancelReject.h>
#include <quickfix/fix50/OrderCancelReplaceRequest.h>
#include <quickfix/fix50/MarketDataRequest.h>

#include <queue>

struct SECURITY
{
    std::string Symbol;
    std::string MDEntryID;
    char MDUpdateAction;
    char MDEntryType;
    double MDEntryPx;
    double MDEntrySize;
    double MinQty;
    double MinInc;
    double MinBR;
    double YTM;
    double YTW;
    SECURITY(){
        MDEntryPx=0;
        MDEntrySize=0;
        MinQty=0;
        MinInc=0;
        MinBR=0;
        YTM=0;
        YTW=0;
    }
    bool operator < (const SECURITY& sec) const {
        if (Symbol < sec.Symbol)
            return true;
        else if (Symbol == sec.Symbol){
            if (MDEntryType < sec.MDEntryType)
                return true;
            else if (MDEntryType == sec.MDEntryType)
                if (MDEntryPx < sec.MDEntryPx)
                    return true;
        }
        return false;
    }
};

class FixApplication :
      public FIX::Application,
      public FIX::MessageCracker
{
public:
    FixApplication(FIX::BeginString beginStr, FIX::SenderCompID senderCID, FIX::TargetCompID targetCID);

    void EnterOrder(std::string id, std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side, FIX::TimeInForce timeInForce);
    void CancelOrder(std::string origId, std::string id, std::string symbol, double orderQty, FIX::Side side);
    void ReplaceOrder(std::string origId, std::string id,std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side);
    void MarketDataRequest();


private:
  void onCreate( const FIX::SessionID& ) {}
  void onLogon( const FIX::SessionID& sessionID );
  void onLogout( const FIX::SessionID& sessionID );
  void toAdmin( FIX::Message&, const FIX::SessionID& );
  void toApp( FIX::Message&, const FIX::SessionID& )
  throw( FIX::DoNotSend );
  void fromAdmin( const FIX::Message&, const FIX::SessionID& )
  throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon ) {}
  void fromApp( const FIX::Message& message, const FIX::SessionID& sessionID )
  throw( FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType );

  void onMessage( const FIX44::ExecutionReport&, const FIX::SessionID& );
  void onMessage( const FIX44::OrderCancelReject&, const FIX::SessionID& );
  void onMessage( const FIX44::MarketDataIncrementalRefresh&, const FIX::SessionID& );
  void onMessage( const FIX50::ExecutionReport&, const FIX::SessionID& );
  void onMessage( const FIX50::OrderCancelReject&, const FIX::SessionID& );

  FIX44::NewOrderSingle NewOrderSingle44(
          std::string id, std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side, FIX::TimeInForce timeInForce);
  FIX50::NewOrderSingle NewOrderSingle50(
          std::string id, std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side, FIX::TimeInForce timeInForce);

  FIX44::OrderCancelRequest OrderCancelRequest44(
          std::string origId, std::string id, std::string symbol, double orderQty, FIX::Side side);
  FIX50::OrderCancelRequest OrderCancelRequest50(
          std::string origId, std::string id, std::string symbol, double orderQty, FIX::Side side);

  FIX44::OrderCancelReplaceRequest CancelReplaceRequest44(
          std::string origId, std::string id, std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side);
  FIX50::OrderCancelReplaceRequest CancelReplaceRequest50(
          std::string origId, std::string id, std::string symbol, double price, double orderQty, FIX::OrdType ordType, FIX::Side side);

  FIX44::MarketDataRequest MarketDataRequest44();
  FIX50::MarketDataRequest MarketDataRequest50();

  std::map<std::string, SECURITY > securities_;

  int version;
  FIX::BeginString beginString;
  FIX::SenderCompID senderCompID;
  FIX::TargetCompID targetCompID;
  FIX::Header header;
};

#endif // FIXAPPLICATION_H
