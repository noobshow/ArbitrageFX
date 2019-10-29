//+------------------------------------------------------------------+
//|                                                     ArbFxMt4.mq4 |
//|                        Copyright 2018, MetaQuotes Software Corp. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2018, MetaQuotes Software Corp."
#property link      "https://www.mql5.com"
#property version   "1.00"
#property strict

// Required: MQL-ZMQ from https://github.com/dingmaotu/mql-zmq
#include <Zmq/Zmq.mqh>

#import "msvcrt.dll"
  int memcpy(char &Destination[], int Source, int Length);
  int memcpy(char &Destination[], long Source, int Length);
  int memcpy(int &dst,  int src, int cnt);
  int memcpy(long &dst,  long src, int cnt);  
#import

extern string  PROJECT_NAME = "ArbFx_PushClient";
extern string  ZEROMQ_PUSH_BIND = "tcp://localhost:5555";
extern int      MILLISECOND_TIMER = 1;  // 1 millisecond
extern bool    CHECK_PRICE_IN_TIMER = true;
extern bool    CHECK_PRICE_IN_TICK = true;
extern string  SYMBOLS_STRING = "AUDJPY,AUDNZD,AUDUSD,AUDCHF,AUDCAD,CADCHF,CADJPY,CHFJPY,EURAUD,EURCAD,EURCHF,EURGBP,EURJPY,EURNZD,EURUSD,GBPAUD,GBPCAD,GBPCHF,GBPJPY,GBPUSD,GBPNZD,NZDCAD,NZDCHF,NZDJPY,NZDUSD,USDCAD,USDCHF,USDJPY";
extern bool    debug = true; 
extern string  symbol_prefix           = ""; 
extern string  symbol_suffix           = "";
const int      CMD_PRICE_ID = 1;

// CREATE ZeroMQ Context
Context context;

// CREATE ZMQ_PUSH SOCKET
Socket pushSocket(context,ZMQ_PUSH);

string listSymbols[];
string listBrokerSymbols[];
int listBid[];
int listAsk[];
double listPoint[];
int symCount, iSym;
int  bid, ask;
string broker;

class packet_struct {
public:
   char broker[3];
   char symbol[6];
   uint bid;
   uint ask;
};

uchar packetArray[];
string packetStr;
//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+
int OnInit()
  {
   if(CHECK_PRICE_IN_TIMER)
      EventSetMillisecondTimer(MILLISECOND_TIMER);     // Set Millisecond Timer to get client socket input
   
   Print(StringFormat("Binding ZeroMQ PUSH Socket : %s", ZEROMQ_PUSH_BIND));
   pushSocket.connect(ZEROMQ_PUSH_BIND);

   string server = AccountInfoString(ACCOUNT_SERVER);
   
   if( StringFind(server, "FortFS", 0) >= 0){
      broker = "FFS";
      symbol_suffix = "f";
   }
   else if( StringFind(server, "ICMarkets", 0) >= 0){
      broker = "ICM";
   }
   else{
      broker = "---";
   }
   
   Print(StringFormat( "OnInit() -> server = %s (%s)", server, broker)); 
   
   symCount =  StringSplit(SYMBOLS_STRING,StringGetCharacter(",",0),listSymbols);
   if(symCount == 0){
      symCount = 1;
      ArrayResize(listSymbols,1);
      listSymbols[0] = Symbol();
   }
   else{
      symCount=ArraySize(listSymbols);
   }
   
   ArrayResize(listBrokerSymbols,symCount);
   ArrayResize(listBid,symCount);
   ArrayInitialize(listBid, 0);
   ArrayResize(listAsk,symCount);
   ArrayInitialize(listAsk, 0);
   ArrayResize(listPoint,symCount);
   ArrayInitialize(listPoint, 0.0);
    
   for(iSym = 0; iSym < symCount; iSym++)
   {
      listBrokerSymbols[iSym] = symbol_prefix + listSymbols[iSym] + symbol_suffix; 
      string currency = StringSubstr( listSymbols[iSym], 3,3); 
      
      if(currency == "XAU") listPoint[iSym] = 0.01;
      else if(currency == "XAG" || currency == "JPY" || currency == "HUF") listPoint[iSym] = 0.001;
      else if(currency == "CZK")  listPoint[iSym] = 0.0001;
      else  listPoint[iSym] = 0.00001;
      
      listBid[iSym] = ToInteger( MarketInfo(listBrokerSymbols[iSym],MODE_BID) );
      listAsk[iSym] = ToInteger( MarketInfo(listBrokerSymbols[iSym],MODE_ASK) );
      
       Print(StringFormat( "OnInit() -> list[%d]: symbol=%s, brokerSymbol=%s, Point=%f, Bid=%d, Ask=%d", 
                                   iSym, listSymbols[iSym], listBrokerSymbols[iSym], listPoint[iSym], listBid[iSym], listAsk[iSym] ));
        
      sendPacket();
   } 
   
   return(INIT_SUCCEEDED);
  }
//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {
//--- destroy timer
   EventKillTimer();
   
   Print(StringFormat("Unbinding ZeroMQ PUSH Socket : %s", ZEROMQ_PUSH_BIND));
   pushSocket.unbind(ZEROMQ_PUSH_BIND);   
  }
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
 {
        
    if(CHECK_PRICE_IN_TICK)
    {
         update();
    }
 }
//+------------------------------------------------------------------+
//| Timer function                                                   |
//+------------------------------------------------------------------+
void OnTimer()
{
   //if(debug){
   //   Print(StringFormat( "OnTimer() -> Milliseconds elapsed: %d", MILLISECOND_TIMER ));
   //}
   update();
}

void sendPacket()
{
       packetStr = StringFormat("%d,%d,%s,%s,%d,%d", CMD_PRICE_ID, getMsElapsed(), broker,  listSymbols[iSym], listBid[iSym], listAsk[iSym]);  
       if(debug)    
         Print(StringFormat( "OnInit() -> sending ZmqMsg: %s",packetStr) );
       
      ZmqMsg message( packetStr, StringLen(packetStr) ); //packetArray, ArraySize(packetArray));
      pushSocket.send(message);
}

void update()
{
   for( iSym = 0; iSym < symCount; iSym++)
   {
      bid = ToInteger( MarketInfo(listBrokerSymbols[iSym],MODE_BID) );
      ask = ToInteger( MarketInfo(listBrokerSymbols[iSym],MODE_ASK) );
      
      if( listBid[iSym] != bid || ask != listAsk[iSym]){    
         listBid[iSym] = bid;
         listAsk[iSym] = ask;
         sendPacket();
      }
   } 
}

int ToInteger(double value){
   return  ( (int) round( value / listPoint[iSym] ) );
}

uint getMsElapsed(){
   return (uint) ( GetMicrosecondCount() /1000);
}
