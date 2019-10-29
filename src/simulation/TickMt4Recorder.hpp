#ifndef TICKMT4RECORDER_H
#define TICKMT4RECORDER_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <thread>
#include <ctime>
#include <chrono>
#include <set>
#include <map>
#include <experimental/filesystem>

#include "common/Observer.hpp"
#include "common/Types.hpp"

namespace fs = std::experimental::filesystem;

struct TickMt4Format{
    std::string path = "TickMt4Recorder/";
    std::string delimiter = ",";
    int minutesPerFile = 1;
    int minuteToWrite = 1;
    bool binary = false;
    bool field_timeStamp = true;
    bool field_broker = true;
    bool field_symbol = true;
    bool field_ask = true;
    bool field_bid = true;
    bool field_spread = true;
    bool field_spreadPercent = true;
    bool field_priceDiff = true;
    bool priceCommissionFix = true;
    bool datetime = true;
    bool realTime = false;
    std::set<string> brokers = {};
    std::set<string> symbols = {};
};

typedef std::pair<int32_t, int32_t> bid_ask;

class TickMt4Recorder : public AsyncObserver<tick_s>
{
private:
    std::vector<TickPtr> buffer_;
    std::map<std::string, bid_ask> lastBrokerPrice;
    TickMt4Format format;
    std::string path;
    std::stringstream filename;
    std::ofstream file;
    std::thread threadProcess;
    std::mutex mutex;
    std::time_t time;
    std::tm* tmPtr;
    int lastWriteMinute, lastFileMinute, lastFileHour;
    chrono::time_point<chrono::system_clock> lastTime;
public:
    TickMt4Recorder(TickMt4Format tickMt4Format);
    ~TickMt4Recorder() override;

    void closeFile() { this->file.close(); }
    void AsyncUpdate(TickPtr tickPtr) override;
    void process();
    void checkFileRecorder();

};
typedef std::shared_ptr<TickMt4Recorder> TickMt4RecorderPtr;

#endif // TICKMT4RECORDER_H
