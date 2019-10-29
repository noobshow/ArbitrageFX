#ifndef IOSTREAM_H
#define IOSTREAM_H

#include "common/Dispatcher.hpp"
#include "common/Types.hpp"

template<typename Data>
class IoStream : public Dispatcher<Data>
{
public:
    virtual void start() = 0;
    virtual void stop() = 0;
};

class TradeMt4Stream : public IoStream<std::string> {};
typedef std::shared_ptr<TradeMt4Stream> TradeMt4StreamPtr;

class PriceMt4Stream : public IoStream<std::string> {};
typedef std::shared_ptr<PriceMt4Stream> PriceMt4StreamPtr;


typedef std::shared_ptr<PriceMt4Stream> PriceMt4StreamPtr;

class TickMt4Stream : public IoStream<tick_s> {};
typedef std::shared_ptr<TickMt4Stream> TickMt4StreamPtr;


#endif // IOSTREAM_H
