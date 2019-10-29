#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

#include "Config.h"
#include "Log.h"

Config::Config()
{
}

Config::Config(std::string fileName)
{
    this->fileName = fileName;
}

Config::~Config()
{
}

void Config::load()
{
    try
    {
        // carrega as configurações do arquivo .ini
        read_ini(this->fileName, this->propertyTree);
        this->socket_port_begin = this->propertyTree.get<int>("socket_port_begin");
        this->socket_port_end = this->propertyTree.get<int>("socket_port_end");

        this->arb_symbols = this->propertyTree.get<std::string>("arb_symbols");

        this->use_full_triangule_arb = this->propertyTree.get<int>("use_full_triangule_arb");
        this->use_currencies_triangule_arb = this->propertyTree.get<int>("use_currencies_triangule_arb");
        this->tri_selected_currencies = this->propertyTree.get<std::string>("tri_selected_currencies");
        this->tri_excluded_currencies = this->propertyTree.get<std::string>("tri_excluded_currencies");

        this->use_full_pair_arb = this->propertyTree.get<int>("use_full_pair_arb");
        this->use_currencies_pair_arb = this->propertyTree.get<int>("use_currencies_pair_arb");
        this->pair_selected_currencies = this->propertyTree.get<std::string>("pair_selected_currencies");
        this->pair_excluded_currencies = this->propertyTree.get<std::string>("pair_excluded_currencies");

        this->use_symbols_pair_arb = this->propertyTree.get<int>("use_symbols_pair_arb");
        this->pair_selected_symbols = this->propertyTree.get<std::string>("pair_selected_symbols");
        this->pair_excluded_symbols = this->propertyTree.get<std::string>("pair_excluded_symbols");

        this->min_pips_match = this->propertyTree.get<int>("min_pips_match");
        this->min_percent_match = this->propertyTree.get<double>("min_percent_match");
        this->max_pips_spread = this->propertyTree.get<int>("max_pips_spread");
        this->max_percent_spread = this->propertyTree.get<double>("max_percent_spread");

        this->min_pips_match = this->propertyTree.get<int>("min_trade_seconds");
        this->min_pips_match = this->propertyTree.get<int>("max_trade_seconds");
        this->min_pips_match = this->propertyTree.get<int>("max_trade_pips");
        this->min_pips_match = this->propertyTree.get<int>("max_trade_margin");
        this->min_pips_match = this->propertyTree.get<int>("max_trade_hour");
        this->min_pips_match = this->propertyTree.get<int>("max_trade_min");
        this->min_pips_match = this->propertyTree.get<int>("max_trade_attempts");
        this->min_pips_match = this->propertyTree.get<int>("msec_attempt_wait");
    }
    catch(ptree_error const&  ex)
    {
        // se arquivo inválido ou inexistente, cria um default
        this->socket_port_begin = 4600;
        this->socket_port_end = 4610;

        this->arb_symbols = "AUDJPY,AUDNZD,AUDUSD,AUDCHF,AUDCAD,CADCHF,CADJPY,CHFJPY,EURAUD,EURCAD,EURCHF,EURGBP,EURJPY,EURNZD,EURUSD,GBPAUD,GBPCAD,GBPCHF,GBPJPY,GBPUSD,GBPNZD,NZDCAD,NZDCHF,NZDJPY,NZDUSD,USDCAD,USDCHF,USDJPY,XAUUSD";
        this->use_full_triangule_arb = 0;
        this->use_currencies_triangule_arb = 0;
        this->tri_selected_currencies = "";
        this->tri_excluded_currencies = "";

        this->use_full_pair_arb = 1;
        this->use_currencies_pair_arb = 0;
        this->pair_selected_currencies = "";
        this->pair_excluded_currencies = "";

        this->use_symbols_pair_arb = 0;
        this->pair_selected_symbols = "";
        this->pair_excluded_symbols = "";

        this->min_pips_match = 5;
        this->min_percent_match = 0.0;
        this->max_pips_spread = 30;
        this->max_percent_spread = 0.0;

        this->min_trade_seconds = 60;
        this->max_trade_seconds = 60*60*2;
        this->max_trade_pips = 200;
        this->max_trade_margin = 0.0;
        this->max_trade_hour = -1;
        this->max_trade_min = -1;
        this->max_trade_attempts = 10;
        this->msec_attempt_wait = 0.0;
        this->save();
    }
    LOG_DEBUG << "Config -> filename: " << this->fileName
              << ", socket_port_begin: " << this->socket_port_begin
              << ", socket_port_end: " << this->socket_port_end;

    LOG_DEBUG << "Config -> arb_symbols: " << this->arb_symbols;

    LOG_DEBUG << "Config -> use_full_triangule_arb: " << this->use_full_triangule_arb
              << ", use_currencies_triangule_arb: " << this->use_currencies_triangule_arb
              << ", tri_selected_currencies: " << this->tri_selected_currencies
              << ", tri_excluded_currencies: " << this->tri_excluded_currencies;

    LOG_DEBUG << "Config -> use_full_pair_arb: " << this->use_full_pair_arb
              << ", use_currencies_pair_arb: " << this->use_currencies_pair_arb
              << ", pair_selected_currencies: " << this->pair_selected_currencies
              << ", pair_excluded_currencies: " << this->pair_excluded_currencies;

    LOG_DEBUG << "Config -> use_symbols_pair_arb: " << this->use_symbols_pair_arb
              << ", pair_selected_symbols: " << this->pair_selected_symbols
              << ", pair_excluded_symbols: " << this->pair_excluded_symbols;

    LOG_DEBUG << "Config -> min_pips_match: " << this->min_pips_match
              << ", min_percent_match: " << this->min_percent_match
              << ", max_pips_spread: " << this->max_pips_spread
              << ", max_percent_spread: " << this->max_percent_spread;

    LOG_DEBUG << "Config -> min_trade_seconds: " << this->min_trade_seconds
              << ", max_trade_seconds: " << this->max_trade_seconds
              << ", max_trade_pips: " << this->max_trade_pips
              << ", max_trade_margin: " << this->max_trade_margin;

    LOG_DEBUG << "Config -> max_trade_hour: " << this->max_trade_hour
              << ", max_trade_min: " << this->max_trade_min
              << ", max_trade_attempts: " << this->max_trade_attempts
              << ", msec_attempt_wait: " << this->msec_attempt_wait;


    this->arbSymbolsSet.clear();
    boost::split(this->arbSymbolsSet,this->arb_symbols,boost::is_any_of(","));

    std::cout << "Symbols Splited: ";
    for( std::string symbol : this->arbSymbolsSet)
        std::cout << symbol << "/";
     std::cout << std::endl;
}

void Config::save()
{
    this->propertyTree.put("socket_port_begin", this->socket_port_begin);
    this->propertyTree.put("socket_port_end", this->socket_port_end);

    this->propertyTree.put("arb_symbols", this->arb_symbols);

    this->propertyTree.put("use_full_triangule_arb", this->use_full_triangule_arb);
    this->propertyTree.put("use_currencies_triangule_arb", this->use_currencies_triangule_arb);
    this->propertyTree.put("tri_selected_currencies", this->tri_selected_currencies);
    this->propertyTree.put("tri_excluded_currencies", this->tri_excluded_currencies);

    this->propertyTree.put("use_full_pair_arb", this->use_full_pair_arb);
    this->propertyTree.put("use_currencies_pair_arb", this->use_currencies_pair_arb);
    this->propertyTree.put("pair_selected_currencies", this->pair_selected_currencies);
    this->propertyTree.put("pair_excluded_currencies", this->pair_excluded_currencies);

    this->propertyTree.put("use_symbols_pair_arb", this->use_symbols_pair_arb);
    this->propertyTree.put("pair_selected_symbols", this->pair_selected_symbols);
    this->propertyTree.put("pair_excluded_symbols", this->pair_excluded_symbols);

    this->propertyTree.put("min_pips_match", this->min_pips_match);
    this->propertyTree.put("min_percent_match", this->min_percent_match);
    this->propertyTree.put("max_pips_spread", this->max_pips_spread);
    this->propertyTree.put("max_percent_spread", this->max_percent_spread);

    this->propertyTree.put("min_trade_seconds", this->min_trade_seconds);
    this->propertyTree.put("max_trade_seconds", this->max_trade_seconds);
    this->propertyTree.put("max_trade_pips", this->max_trade_pips);
    this->propertyTree.put("max_trade_margin", this->max_trade_margin);

    this->propertyTree.put("max_trade_hour", this->max_trade_hour);
    this->propertyTree.put("max_trade_min", this->max_trade_min);
    this->propertyTree.put("max_trade_attempts", this->max_trade_attempts);
    this->propertyTree.put("msec_attempt_wait", this->msec_attempt_wait);

    write_ini(this->fileName, this->propertyTree);
}

void Config::setFileName(std::string value){
    this->fileName = value;
}
std::string Config::getFileName(){
    return this->fileName;
}

