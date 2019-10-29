#ifndef OBSERVER_H
#define OBSERVER_H

#include <condition_variable>
#include <memory>
#include <vector>

#include "common/AsyncThreadProcess.hpp"

using namespace std;

template<class Data> class Observer
{
public:
    virtual ~Observer() = default;
    virtual void Update(std::shared_ptr<Data> dataPtr) = 0;
};

template<class Data> class AsyncObserver :
        public Observer<Data>,
        public AsyncThreadProcess<Data>
{
public:
    virtual ~AsyncObserver() = default;
    void Update(std::shared_ptr<Data> dataPtr){
        this->addData(dataPtr);
    }
    virtual void AsyncUpdate(std::shared_ptr<Data> dataPtr) = 0;
};

template<class Data> class PoolObserver : public Observer<Data>
{
public:
    PoolObserver() {
        this->running_ = false;
        this->threads_ = 0;
    }
    virtual ~PoolObserver() {
        this->Stop();
    }
    void Start(int threads) {
        this->threads_ = threads;
        if(this->threads_ > 0){
            this->running_ = true;
            for(int i = 0; i < this->threads_; i++)
            {
                this->pool_.push_back(std::thread(&PoolObserver::process, this));
            }
        }
    }
    void Stop() {
        this->running_ = false;
        for(int i = 0; i < this->threads_; i++)
        {
            if(this->pool_[i].joinable())
                this->pool_[i].join();
        }
    }
    void Update(std::shared_ptr<Data> dataPtr)
    {
        this->mutex_buffer_.lock();
        this->buffer_.push_back(dataPtr);
        this->mutex_buffer_.unlock();
        this->condVar_.notify_one();
    }
    virtual void AsyncUpdate(std::shared_ptr<Data> dataPtr) = 0;
private:
    int threads_;
    std::atomic<bool> running_;
    vector<std::thread> pool_;
    std::mutex mutex_loop_, mutex_buffer_;
    std::condition_variable condVar_;
    std::deque<std::shared_ptr<Data>> buffer_;

    void process(){
        bool update;
        std::shared_ptr<Data> dataPtr;
        while( this->running_ )
        {
            update = false;
            this->mutex_buffer_.lock();
            if( this->buffer_.empty() )
            {
                this->mutex_buffer_.unlock();
                std::unique_lock<std::mutex> lock(this->mutex_loop_);
                this->condVar_.wait_for(lock, std::chrono::milliseconds(LOOP_WAIT_MS));
            }
            else
            {
                dataPtr = this->buffer_.front();
                this->buffer_.pop_front();
                this->mutex_buffer_.unlock();
                update = true;
            }
            if(update)
                this->AsyncUpdate(dataPtr);
        }
    }
};

#endif // OBSERVER_H

