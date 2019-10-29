//+------------------------------------------------------------------+
//|                                              ArbFx_SubClient.mq4 |
//|                        Copyright 2018, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2018, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"
#property strict

// Required: MQL-ZMQ from https://github.com/dingmaotu/mql-zmq
#include <Zmq/Zmq.mqh>

extern string  PROJECT_NAME = "ArbFx_SubClient";
extern string  ZEROMQ_BIND_SUB = "tcp://localhost:5557";
extern string  ZEROMQ_SUBSCRIBE = "ICM";
extern string  ZEROMQ_BIND_PUSH = "tcp://localhost:5556";
extern string  symbol_prefix           = ""; 
extern string  symbol_suffix           = "";
extern double VOLUME = 0;
extern int     magic = 123456;
extern bool    debug = true; 

static const int CMD_OPEN_ORDER_REQ = 2;
static const int CMD_OPEN_ORDER_RESP = 3;
static const int CMD_CLOSE_ORDER_REQ = 4;
static const int CMD_CLOSE_ORDER_RESP = 5;

static const int ORDER_CREATED = 1;
static const int ORDER_SEND = 2;
static const int ORDER_FILLED = 3;
static const int ORDER_ERROR = 4;
static const int ORDER_TIMEOUT = 5;
static const int ORDER_DISABLED = 6;

// CREATE ZeroMQ Context
Context context;
// CREATE ZMQ_SUB SOCKET
Socket subSocket(context,ZMQ_SUB);

// CREATE ZMQ_PUSH SOCKET
Socket pushSocket(context,ZMQ_PUSH);
string packetStr;

//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {
  Print(StringFormat("Binding ZeroMQ SUB Socket : %s %s", ZEROMQ_SUBSCRIBE,  ZEROMQ_BIND_SUB));
   subSocket.connect(ZEROMQ_BIND_SUB);
   subSocket.subscribe(ZEROMQ_SUBSCRIBE);
   
   Print(StringFormat("Binding ZeroMQ PUSH Socket : %s", ZEROMQ_BIND_PUSH));
   pushSocket.connect(ZEROMQ_BIND_PUSH);
   
   OnTick();
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
   
   Print("Unbinding SUBSCRIBE ZeroMQ Socket: " + ZEROMQ_BIND_SUB + "..");
   subSocket.unbind(ZEROMQ_BIND_SUB);   
   
      Print(StringFormat("Unbinding ZeroMQ PUSH Socket : %s", ZEROMQ_BIND_PUSH));
   pushSocket.unbind(ZEROMQ_BIND_PUSH);   
  }

//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
 {      
   while(!IsStopped())
   {
      ZmqMsg message;

      subSocket.recv(message);
      
      string msg = message.getData();
      string msg_array[];
      int tokens = StringSplit(msg,',',msg_array);
      
      if(tokens <= 0){
         Print( "Invalid received Message: " + msg);
         continue;
      }
      int cmd = StringToInteger(msg_array[0]);
      switch(cmd)
     {
      case 2:
      {
         Print("Received command CMD_OPEN_ORDER_REQ: " + msg);
         if( tokens != 11){
            Print("Error command CMD_OPEN_ORDER_REQ: Tokens != 11");
            break;
         }
         int order_id = StrToInteger(msg_array[1]);
         int trade_id = StrToInteger(msg_array[2]);
         string broker = msg_array[3];
         string symbol = symbol_prefix + msg_array[4] + symbol_suffix;
         double point = MarketInfo(symbol, MODE_POINT);
         int digits =  MarketInfo(symbol, MODE_DIGITS);
         int order_type = StrToInteger(msg_array[5]);
         double volume = StrToDouble(msg_array[6]);
         int price_int = StrToInteger(msg_array[7]);
         double price = NormalizeDouble( (price_int * point ) , digits) ;
         int slippage = StrToInteger(msg_array[8]);
         double stoploss = StrToInteger(msg_array[9]);
         if(stoploss > 0) stoploss = NormalizeDouble( (stoploss * point) , digits) ;
         double takeprofit = StrToInteger(msg_array[10]);
         if(takeprofit > 0) takeprofit = NormalizeDouble( (takeprofit * point) , digits) ;
         
          Print(StringConcatenate("CMD VALUES: OrderId: ", order_id, ", TradeID: ", trade_id, ", broker: ", broker,
               ", symbol: ", symbol, ", order_type: ", order_type, ", volume: ", volume, ", price: ", price, ", slippage: ",
               slippage, ", stoploss: ", stoploss, ", takeprofit: ", takeprofit ));
               
         if( broker == ZEROMQ_SUBSCRIBE)
         {
             string comment = "";
             if(VOLUME > 0)  volume = VOLUME;
             int ticket = OrderSend(symbol, order_type, volume, price, slippage, stoploss, takeprofit, comment, magic);
             
             if(ticket<0)
             {
                Print("OrderSend failed with error #",GetLastError());
                
                 packetStr = StringFormat("%d,%d,%d,%d,%d,%d", CMD_OPEN_ORDER_RESP, order_id, trade_id, ticket, ORDER_ERROR, 0);  
                 Print(StringFormat( "OnInit() -> sending push ZmqMsg: %s",packetStr) );
                
                  ZmqMsg msg_send( packetStr, StringLen(packetStr) );
                  pushSocket.send(msg_send);
             }
            else
               Print("OrderSend placed successfully");
                int price_filled = price_int;
               if(OrderSelect(ticket, SELECT_BY_TICKET)==true)
               {
                   price_filled =  (int) round( OrderOpenPrice() / point);
                   Print(StringFormat("order %d open price is %d", ticket,  price_filled));
               }
               else{
                   Print("OrderSelect returned the error of ",GetLastError());
               }
               
               packetStr = StringFormat("%d,%d,%d,%d,%d,%d", CMD_OPEN_ORDER_RESP, order_id, trade_id, ticket, ORDER_FILLED, price_filled);  
                 Print(StringFormat( "OnInit() -> sending push ZmqMsg: %s",packetStr) );
                
                  ZmqMsg msg_send( packetStr, StringLen(packetStr) );
                  pushSocket.send(msg_send);
             }
             else{
                Print(" Invalid Broker Command: " + broker);
             }
         break;
      }
      case 4:
      {
         Print("Received Command CMD_CLOSE_ORDER_REQ: " + msg);
         if( tokens != 9){
            Print("Error command CMD_CLOSE_ORDER_REQ: Tokens != 9");
            break;
         }
         int order_id = StrToInteger(msg_array[1]);
         int trade_id = StrToInteger(msg_array[2]);
         int ticket = StrToInteger(msg_array[3]);
         string broker = msg_array[4];
         string symbol = symbol_prefix + msg_array[5] + symbol_suffix;
         double point = MarketInfo(symbol, MODE_POINT);
          int digits =  MarketInfo(symbol, MODE_DIGITS);
         double volume = StrToDouble(msg_array[6]);
         int price_int = StrToInteger(msg_array[7]);
         double price = NormalizeDouble( (price_int * point ) , digits) ;
         int slippage = StrToInteger(msg_array[8]);
         Print(StringConcatenate("CMD VALUES: OrderId: ", order_id, ", tradeId: ", trade_id, ", Ticket: ", ticket, 
         ", broker: ", broker, ", symbol: ", symbol, ", volume: ", volume, ", price: ", price, ", slippage: ", slippage ));
          
          if( broker == ZEROMQ_SUBSCRIBE)
          {
             if(VOLUME > 0)  volume = VOLUME;
             if(OrderClose(ticket, volume, price, slippage))
             {
                  Print("OrderClose executed successfully");
               int price_filled = price_int;
               if(OrderSelect(ticket, SELECT_BY_TICKET)==true)
               {
                   price_filled =  (int) round( OrderClosePrice() / point);
                   Print(StringFormat("order %d open price is %d", ticket,  price_filled));
               }
               else{
                   Print("OrderSelect returned the error of ",GetLastError());
               }
               
               packetStr = StringFormat("%d,%d,%d,%d,%d,%d", CMD_OPEN_ORDER_RESP, order_id, trade_id, ticket, ORDER_FILLED, price_filled);  
                 Print(StringFormat( "OnInit() -> sending push ZmqMsg: %s",packetStr) );
                
                  ZmqMsg msg_send( packetStr, StringLen(packetStr) );
                  pushSocket.send(msg_send);
             }
            else
            {
                Print("OrderClose failed with error #",GetLastError());
                
                 packetStr = StringFormat("%d,%d,%d,%d,%d,%d", CMD_OPEN_ORDER_RESP, order_id, trade_id, ticket, ORDER_ERROR, 0);  
                 Print(StringFormat( "OnInit() -> sending push ZmqMsg: %s",packetStr) );
                
                  ZmqMsg msg_send( packetStr, StringLen(packetStr) );
                  pushSocket.send(msg_send);
            }
         }
         else{
             Print(" Invalid Broker Command: " + broker);
         }
         break;
      }
      default:
         Print( "Invalid Command Message: " + msg);
         break;
     }
   }
 }

/*
CMD_OPEN_ORDER_REQ:
           << CMD_OPEN_ORDER_REQ << DELIMITER << order_id_ << DELIMITER << trade_id_
           << DELIMITER << broker_ << DELIMITER << symbol_ << DELIMITER << order_type_
           << DELIMITER << volume_ << DELIMITER << price_ << DELIMITER << slippage_
           << DELIMITER << stoploss_ << DELIMITER << takeprofit_;
           
CMD_OPEN_ORDER_RESP:
       << CMD_OPEN_ORDER_RESP << DELIMITER << openOrderPtr->order_id_ << DELIMITER
       << openOrderPtr->trade_id_ << DELIMITER << status << DELIMITER << openOrderPtr->price_;
       
CMD_CLOSE_ORDER_REQ:
           << CMD_CLOSE_ORDER_REQ << DELIMITER << order_id_ << DELIMITER
           << trade_id_ << DELIMITER << broker_ << DELIMITER << symbol_
           << DELIMITER << volume_ << DELIMITER << price_ << DELIMITER << slippage_;
CMD_CLOSE_ORDER_RESP:
       << CMD_CLOSE_ORDER_RESP << DELIMITER << closeOrderPtr->order_id_ << DELIMITER
       << closeOrderPtr->trade_id_ << DELIMITER << status << DELIMITER << closeOrderPtr->price_;
       
       
//+------------------------------------------------------------------+
void OpenOrder(int _orderType, double tp, double sl, double _lots = 0){
   bool isBuy = false, isSell = false;
   string orderType = OrderTypeToString(_orderType);
   comment = IntegerToString(iSetup);

   if(_lots == 0) _lots = (Risk > 0 ? lotSizeByRisk(iSymbol, iSetup, _orderType) : Lots);
       
   double price = 0;
   
   if(_orderType == OP_BUY || _orderType == OP_BUYLIMIT || _orderType == OP_BUYSTOP){
      price = ASK;
      isBuy = true;
   }
   else if(_orderType == OP_SELL || _orderType == OP_SELLLIMIT || _orderType == OP_SELLSTOP){
      price = BID;
      isSell = true;
   }
   else { 
      logMessage(LOG_ERROR, symbol, timeframe, "OpenOrder", StringConcatenate("Order type not exist: ",_orderType)); return; 
   }
   
   logMessage(LOG_INFO, symbol, timeframe, "OpenOrder", StringConcatenate("Opening order... Type: ",orderType,", Price:",price)); 

   ticket=OrderSend(symbol,_orderType,_lots,price,3,0,0,comment,Magic,0,(_orderType == OP_BUY ? clrGreen : clrRed));
   if(ticket>0)
   {
      if(OrderSelect(ticket,SELECT_BY_TICKET,MODE_TRADES)){
         logMessage(LOG_DEBUG, symbol, timeframe, "OpenOrder", StringConcatenate("Order opened! Ticket: ",ticket,", Price: ",OrderOpenPrice(),", Time:",TimeToStr(OrderOpenTime(),TIME_DATE|TIME_SECONDS)));      
         //logMessage(LOG_WARN, _symbol, timeframe, "OpenOrder", StringConcatenate("ADD ORDER: type: ",_orderType,", setup: ",_iSetup));
         addOrderInMemory(new Order(ticket, iSymbol, iSetup, _orderType, OrderLots(), OrderProfit(), OrderCommission(), OrderSwap(), OrderOpenTime()));
         
         if(isBuy) { lastBuyOrderBySetup[iSymbol][iSetup] = 0;  lastBuyOrderBySymbol[iSymbol] = 0; }
         if(isSell) { lastSellOrderBySetup[iSymbol][iSetup] = 0; lastSellOrderBySymbol[iSymbol] = 0; }
         
         if(tp > 0 || sl > 0){
            while (true) {
               if (!OrderModify(ticket, 0, sl, tp, 0, Blue)){
                  logMessage(LOG_ERROR, symbol, timeframe, "OpenOrder", StringConcatenate("OrderModify return error number ",GetLastError()));
               }
               break;
            }  
         }
      }
      else{
         logMessage(LOG_ERROR, symbol, timeframe, "OpenOrder", "Error opening order: ticket not valid!");
      }
   }
   else{
      logMessage(LOG_ERROR, symbol, timeframe, "OpenOrder", StringConcatenate("Error opening order - error Code: ",GetLastError()));
   }
}


void sendPacket()
{
       packetStr = StringFormat("%d,%d,%s,%s,%d,%d", CMD_PRICE_ID, getMsElapsed(), broker,  listSymbols[iSym], listBid[iSym], listAsk[iSym]);  
       if(debug)    
         Print(StringFormat( "OnInit() -> sending ZmqMsg: %s",packetStr) );
       
      ZmqMsg message( packetStr, StringLen(packetStr) ); //packetArray, ArraySize(packetArray));
      pushSocket.send(message);
}

int ToInteger(double value){
   return  ( (int) round( value / listPoint[iSym] ) );
}

uint getMsElapsed(){
   return (uint) ( GetMicrosecondCount() /1000);
}
*/

/*
int  OrderSend(
   string   symbol,              // symbol
   int      cmd,                 // operation
   double   volume,              // volume
   double   price,               // price
   int      slippage,            // slippage
   double   stoploss,            // stop loss
   double   takeprofit,          // take profit
   string   comment=NULL,        // comment
   int      magic=0,             // magic number
   datetime expiration=0,        // pending order expiration
   color    arrow_color=clrNONE  // color
);

bool  OrderClose(
   int        ticket,      // ticket
   double     lots,        // volume
   double     price,       // close price
   int        slippage,    // slippage
   color      arrow_color  // color
);

struct open_order_cmd_s
{
    int32_t order_id_;
    int32_t trade_id_;
    std::string broker_;
    std::string symbol_;
    int32_t order_type_;
    double volume_;
    int32_t price_;
    int32_t slippage_;
    int32_t stoploss_;
    int32_t takeprofit_;

    CMD << CMD_OPEN_ORDER_REQ (2) << order_id_ << trade_id_ << broker_ << symbol_ << order_type_
        << volume_ << price_ << slippage_ << stoploss_ << takeprofit_;
}

struct close_order_cmd_s
{
    int32_t order_id_;
    int32_t trade_id_;
    std::string broker_;
    std::string symbol_;
    double volume_;
    int32_t price_;
    int32_t slippage_;

    CMD << CMD_CLOSE_ORDER_REQ(4) << order_id_  trade_id_ << broker_  << symbol_
        << volume_ << price_ << slippage_;
}

struct order_resp_cmd_s
{
    int32_t cmd_type_;
    int32_t order_id_;
    int32_t trade_id_;
    int32_t order_status_;
    int32_t price_;
}
const std::string DELIMITER = ",";
*/