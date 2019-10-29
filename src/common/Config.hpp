#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <iostream>
#include <fstream>
#include <set>
#include <QString>
#include <QStringList>

#include "inicpp.hpp"
#include "common/Singleton.hpp"


class Config : public Singleton<Config>
{
public:
    Config();
    void load();
    void save();

    std::string fileName;
    std::string referenceBroker;
    std::string price_mt4_pull_bind;
    std::string trade_mt4_pull_bind;
    std::string trade_mt4_pub_bind;
    bool use_pair_arbitrage;
    std::string pair_tradable_broker;
    int pair_threads;
    int pair_volume;
    int pair_volume_mult;
    int pair_slippage;
    int pair_points_open;
    int pair_close_points_begin;
    int pair_close_points_end;
    int pair_close_sec_step;
    int pair_close_sec_mult;
    bool use_triangle_arbitrage;
    int symbol_max_buffer;
    int symbol_spread_buffer;

    std::set<std::string> pairTradableBrokerSet;
    template<typename T>
    T loadEntry(std::string group, std::string fieldName, T defaultValue);
protected:
    ini::IniFile iniFile;
};

#endif // CONFIGFILE_H
