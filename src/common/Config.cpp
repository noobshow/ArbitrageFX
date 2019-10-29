
#include "common/Config.hpp"
#include "common/loguru.hpp"


Config::Config()
{

}

void Config::load()
{
    std::ifstream file (this->fileName);
    if (file.is_open())
    {
        this->iniFile.decode(file);
    }
    else{
        LOG_ERROR << "Erro ao abrir arquivo INI para leitura: " << this->fileName;
    }
    std::string group = "arbitrage";

    this->referenceBroker = this->loadEntry<std::string>(group, "referenceBroker", "F4Y" );
    this->price_mt4_pull_bind = this->loadEntry<std::string>(group, "price_mt4_pull_bind", "tcp://*:5555" );
    this->trade_mt4_pull_bind = this->loadEntry<std::string>(group, "trade_mt4_pull_bind", "tcp://*:5556" );
    this->trade_mt4_pub_bind = this->loadEntry<std::string>(group, "trade_mt4_pub_bind", "tcp://*:5557" );
    this->use_pair_arbitrage = this->loadEntry<bool>(group, "use_pair_arbitrage", true );
    this->pair_tradable_broker = this->loadEntry<std::string>(group, "pair_tradable_broker", "FFS,F4Y,ICM,TW4" );
    this->pair_threads = this->loadEntry<int>(group, "pair_threads", 2 );
    this->pair_volume = this->loadEntry<int>(group, "pair_volume", 1 );
    this->pair_volume_mult = this->loadEntry<int>(group, "pair_volume_mult", 14 );
    this->pair_slippage = this->loadEntry<int>(group, "pair_slippage", 2 );
    this->pair_points_open = this->loadEntry<int>(group, "pair_points_open", 10 );
    this->pair_close_points_begin = this->loadEntry<int>(group, "pair_close_points_begin", -9 );
    this->pair_close_points_end = this->loadEntry<int>(group, "pair_close_points_end", 9 );
    this->pair_close_sec_step = this->loadEntry<int>(group, "pair_close_sec_step", 60 );
    this->pair_close_sec_mult = this->loadEntry<int>(group, "pair_close_sec_mult", 13 );
    this->use_triangle_arbitrage = this->loadEntry<bool>(group, "use_triangle_arbitrage", true );
    this->symbol_max_buffer = this->loadEntry<int>(group, "symbol_max_buffer", 1000 );
    this->symbol_spread_buffer = this->loadEntry<int>(group, "symbol_spread_buffer", 1000 );

    this->save();

    this->pairTradableBrokerSet.clear();
    QString symbols(this->pair_tradable_broker.c_str());
    QStringList tokens = symbols.split(",");
    for(QString token : tokens){
        this->pairTradableBrokerSet.insert(token.toStdString());
    }

    std::cout << "Tradable Brokers Splited: ";
    for( std::string symbol : this->pairTradableBrokerSet)
        std::cout << symbol << "/";
    std::cout << std::endl;
}

void Config::save()
{
    std::string group = "arbitrage";
    this->iniFile[group]["referenceBroker"] = this->referenceBroker;
    this->iniFile[group]["price_mt4_pull_bind"] = this->price_mt4_pull_bind;
    this->iniFile[group]["trade_mt4_pull_bind"] = this->trade_mt4_pull_bind;
    this->iniFile[group]["trade_mt4_pub_bind"] = this->trade_mt4_pub_bind;
    this->iniFile[group]["use_pair_arbitrage"] = this->use_pair_arbitrage;
    this->iniFile[group]["pair_tradable_broker"] = this->pair_tradable_broker;
    this->iniFile[group]["pair_threads"] = this->pair_threads;
    this->iniFile[group]["pair_volume"] = this->pair_volume;
    this->iniFile[group]["pair_volume_mult"] = this->pair_volume_mult;
    this->iniFile[group]["pair_slippage"] = this->pair_slippage;
    this->iniFile[group]["pair_points_open"] = this->pair_points_open;
    this->iniFile[group]["pair_close_points_begin"] = this->pair_close_points_begin;
    this->iniFile[group]["pair_close_points_end"] = this->pair_close_points_end;
    this->iniFile[group]["pair_close_sec_step"] = this->pair_close_sec_step;
    this->iniFile[group]["pair_close_sec_mult"] = this->pair_close_sec_mult;
    this->iniFile[group]["use_triangle_arbitrage"] = this->use_triangle_arbitrage;
    this->iniFile[group]["symbol_max_buffer"] = this->symbol_max_buffer;
    this->iniFile[group]["symbol_spread_buffer"] = this->symbol_spread_buffer;

    std::ofstream file (this->fileName);
    if (file.is_open())
    {
        std::string encodeStr = this->iniFile.encode();
        file << encodeStr;
        file.close();
    }
    else{
        LOG_ERROR << "Erro ao abrir arquivo INI para escrita: " << this->fileName;
    }
}


template<typename T>
T Config::loadEntry(std::string group, std::string fieldName, T defaultValue)
{
    T value;
    if( this->iniFile[group].find(fieldName) != this->iniFile[group].end()){
        value = this->iniFile[group][fieldName].as<T>();
        LOG_INFO << "INI Settings -> [" << group << "][" << fieldName << "] Found. Value: " << value;
    }
    else{
        this->iniFile[group][fieldName] = value = defaultValue;
        LOG_INFO << "INI Settings ->  [" << group << "][" << fieldName << "] Not Found. Default Value: " << value;
    }
    return value;
}

