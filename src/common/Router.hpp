#ifndef ROUTERDISPATCHER_H
#define ROUTERDISPATCHER_H

#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <memory>

#include "common/Observer.hpp"

template<class Data> class Router
{
private:
protected:
    std::unordered_map<std::string, std::shared_ptr<Observer<Data>>> map;
    bool working;
public:
    Router(): working(false) {}
    virtual ~Router() {}
    bool isWorking() { return this->working; }

    void Attach(std::string key, std::shared_ptr<Observer<Data>> observer){
        map.insert(std::make_pair(key, observer));
    }
    void Detach(std::string key){
        map.erase(key);
    }

    void Notify(std::string key, std::shared_ptr<Data> data)
    {
        auto it = map.find(key);
        if( it != map.end()){
            it->second->Update(data);
        }
        else{
            LOG_WARNING << "Router not find key " << key << ", TypeData: " << typeid(Data).name();
        }
    }

};

template<class Data> class RouterMultiIndex
{
private:
protected:
    std::unordered_map<std::string, vector<std::shared_ptr<Observer<Data>>>> multimap;
    bool working;
public:
    RouterMultiIndex(): working(false) {}
    virtual ~RouterMultiIndex() {}
    bool isWorking() { return this->working; }

    void Attach(std::string key, std::shared_ptr<Observer<Data>> observer)
    {
        auto it = multimap.find(key);
        if( it != multimap.end()){
            it->second.push_back( observer );
        }
        else{
            multimap[key] = {observer};
        }

    }
    void Detach(std::string key){
        multimap.erase(key);
    }

    void Notify(std::string key, std::shared_ptr<Data> data)
    {
        auto it = multimap.find(key);
        if( it != multimap.end())
        {
            for( std::shared_ptr<Observer<Data>> observer : it->second ){
                observer->Update(data);
            }
        }
        else{
            LOG_WARNING << "Router not find key " << key << ", TypeData: " << typeid(Data).name();
        }
    }

};

#endif // ROUTERDISPATCHER_H
