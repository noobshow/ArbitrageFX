#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <iostream>
#include <set>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "Singleton.h"

using namespace boost::property_tree;

class Config :
        public Singleton<Config>
{
public:
    Config(std::string fileName);
    Config();

    virtual ~Config();
    void load();
    void setFileName(std::string value);
    std::string getFileName();
    std::set<std::string> arbSymbolsSet;

    // sockets parameters
    int socket_port_begin;
    int socket_port_end;

    std::string arb_symbols;
    // triangule arbitrage parameters
    int use_full_triangule_arb;
    int use_currencies_triangule_arb;
    std::string tri_selected_currencies;
    std::string tri_excluded_currencies;

    // triangule arbitrage parameters
    int use_full_pair_arb;
    int use_currencies_pair_arb;
    std::string pair_selected_currencies;
    std::string pair_excluded_currencies;
    int use_symbols_pair_arb;
    std::string pair_selected_symbols;
    std::string pair_excluded_symbols;

    // match arbitrage parameters
    int min_pips_match;
    double min_percent_match;
    int max_pips_spread;
    double max_percent_spread;

    int min_trade_seconds;
    int max_trade_seconds;
    int max_trade_pips;
    int max_trade_margin;
    int max_trade_hour;
    int max_trade_min;
    int max_trade_attempts;
    int msec_attempt_wait;

protected:
private:
    std::string fileName;
    ptree propertyTree;

    void save();

};

#endif // CONFIGFILE_H
