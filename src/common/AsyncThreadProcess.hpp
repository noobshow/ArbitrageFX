#ifndef AsyncThreadProcess_H
#define AsyncThreadProcess_H

#include <thread>
#include <atomic>
#include <memory>
#include <deque>
#include <mutex>
#include <chrono>
#include <condition_variable>

#include "common/loguru.hpp"

const uint32_t MAX_LIST_SIZE = 100000;
const uint32_t LIST_SIZE_WARNING = 5;
const uint32_t LOOP_WAIT_MS = 500;


template<class Data>
class AsyncThreadProcess
{
private:
    std::mutex mutex, mutexList;
    std::condition_variable condVar;
    std::thread threadProcess;
    uint32_t loop_wait_ms;
    uint32_t list_size_warning;
    uint32_t max_list_size;
    uint32_t list_size;
    bool simulation_mode;
protected:
    std::deque<std::shared_ptr<Data>> buffer;
    std::atomic<bool> running;

    ~AsyncThreadProcess(){
        this->running = false;
        if(this->threadProcess.joinable())
            this->threadProcess.join();
    }

    bool IsBufferFull()
    {
        this->mutexList.lock();
        bool isBufferFull = this->buffer.size() >= this->max_list_size;
        this->mutexList.unlock();
        return isBufferFull;
    }

    bool IsBufferEmpty()
    {
        this->mutexList.lock();
        bool isBufferEmpty = this->buffer.empty();
        this->mutexList.unlock();
        return isBufferEmpty;
    }

    void process(){
        std::unique_lock<std::mutex> lock(this->mutex);
        while( this->running )
        {
            this->condVar.wait_for(lock, std::chrono::milliseconds(this->loop_wait_ms));

            while( !IsBufferEmpty() )
            {
                this->mutexList.lock();
                std::shared_ptr<Data> dataPtr = this->buffer.front();
                this->buffer.pop_front();
                this->mutexList.unlock();

                this->AsyncUpdate(dataPtr);
            }
        }
    }

public:
    AsyncThreadProcess(){
        this->running = true;
        this->max_list_size = MAX_LIST_SIZE;
        this->list_size_warning = LIST_SIZE_WARNING;
        this->loop_wait_ms = LOOP_WAIT_MS;
        this->threadProcess = std::thread(&AsyncThreadProcess::process, this);
    }

    void stop() { this->running = false; }
    bool isRunning() { return this->running; }
    bool isProcessing() { return ( buffer.size() > 0 ); }
    void setSimulatioMode(){
        this->list_size_warning = 0;
    }

    void ThreadConfigure(uint32_t max_list_size, uint32_t list_size_warning, uint32_t loop_wait_ms)
    {
        this->max_list_size = max_list_size;
        this->list_size_warning = list_size_warning;
        this->loop_wait_ms = loop_wait_ms;
    }

    void addData(std::shared_ptr<Data> dataPtr)
    {

        while(true)
        {
            if( IsBufferFull() ){
                LOG_ERROR << "Buffer<" << typeid(Data).name() << "> está cheio. Aguardando processamento...";
                std::this_thread::sleep_for (std::chrono::seconds(1));
            }
            else{
                if(this->list_size_warning > 0 && this->list_size >= this->list_size_warning){
                    LOG_WARNING << "Buffer<" << typeid(Data).name() << "> com tamanho acima do normal "
                                << this->list_size << " > " << this->list_size_warning;
                    std::this_thread::sleep_for (std::chrono::seconds(1));
                }
                break;
            }
        }
        this->mutexList.lock();
        this->buffer.push_back(dataPtr);
        this->mutexList.unlock();
        this->condVar.notify_all();
    }

    virtual void AsyncUpdate(std::shared_ptr<Data> dataPtr) = 0;

};

#endif // AsyncThreadProcess_H
