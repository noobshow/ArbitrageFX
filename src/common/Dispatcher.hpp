#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <vector>
#include <memory>

#include "common/Observer.hpp"

template<class Data> class Dispatcher
{
private:
protected:
    std::vector<std::shared_ptr<Observer<Data>>> list;
    bool working;
public:
    Dispatcher(): working(false) {}
    virtual ~Dispatcher() {}
    bool isWorking() { return this->working; }

    void Attach(std::shared_ptr<Observer<Data>> observer){
        list.push_back(observer);
    }
    void Detach(std::shared_ptr<Observer<Data>> observer){
        list.erase(std::remove(list.begin(), list.end(), observer), list.end());
    }

    void Notify(std::shared_ptr<Data> data)
    {
        for(auto observer : list)
        {
            observer->Update(data);
        }
    }

};

#endif // DISPATCHER_H
