#include <sstream>
#include <cmath>

#include "common/Symbol.hpp"
#include "common/Config.hpp"

Symbol::Symbol(std::string symbol, std::string broker) :
    symbol_(symbol), broker_(broker)
{
    this->max_buffer_size_ = static_cast<uint32_t>(Config::getInstance()->symbol_max_buffer);
    this->spread_buffer_size_ =static_cast<uint32_t>(Config::getInstance()->symbol_spread_buffer);
    this->spread_buffer_sum_ = 0.0;
    this->spread_buffer_avg_ = 0.0;
    this->spread_buffer_.clear();
}

void Symbol::updateSymbol(TickPtr tickPtr)
{
    this->mutex_update_.lock();
    //this->ticks.insert(std::make_pair(tickPtr->timestamp_, price_s{tickPtr->bid_, tickPtr->ask_}));
    this->lastTickPtr = tickPtr;

    if( this->spread_buffer_.size() >= this->spread_buffer_size_ )
    {
        this->spread_buffer_sum_ -= this->spread_buffer_.front();
        this->spread_buffer_.pop_front();
    }

    double spread = tickPtr->ask_ - tickPtr->bid_;
    this->spread_buffer_.push_back( spread );
    this->spread_buffer_sum_ += spread;
    this->spread_buffer_avg_ = this->spread_buffer_sum_ / static_cast<int32_t>(this->spread_buffer_.size());
    this->mutex_update_.unlock();
}

int32_t Symbol::getSpreadAvg()
{
    return static_cast<int32_t>(std::ceil(this->spread_buffer_avg_));
}

TickPtr Symbol::getLastTick()
{
    this->mutex_update_.lock();
    TickPtr tickPtr = this->lastTickPtr;
    this->mutex_update_.unlock();
    return tickPtr;
}

