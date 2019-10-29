#ifndef RAWTICKMT4RECORDER_H
#define RAWTICKMT4RECORDER_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <thread>
#include <ctime>
#include <chrono>
#include <experimental/filesystem>

#include "common/Observer.hpp"

namespace fs = std::experimental::filesystem;
using namespace std;

class MuxMt4Recorder :
        public AsyncObserver<std::string>
{
private:
    string filename, path;
    ofstream file;
    vector<std::string> cmd_buffer;
    std::thread threadProcess;
    std::mutex mutex;
    std::time_t time;
    std::tm* tmPtr;
    int lastMinute, lastHour, timeInMinutes;
    chrono::time_point<chrono::system_clock> lastTime;
public:
    MuxMt4Recorder(string path, int minutesPerFile);
    ~MuxMt4Recorder() override;

    void closeFile() { this->file.close(); }
    void AsyncUpdate(std::shared_ptr<string> stringPtr) override;
    void process();

};
typedef std::shared_ptr<MuxMt4Recorder> MuxMt4RecorderPtr;

#endif // RAWTICKMT4RECORDER_H
