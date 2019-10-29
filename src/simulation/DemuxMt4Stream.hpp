#ifndef RAWTICKMT4FILESTREAM_H
#define RAWTICKMT4FILESTREAM_H

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <memory>

#include "common/Router.hpp"
#include "common/loguru.hpp"
#include "common/Types.hpp"

//class DemuxMt4Stream : public RouterM<std::string>
class DemuxMt4Stream : public RouterMultiIndex<std::string>
{
private:
    vector<string> files;
    thread threadProccessFiles;
    void processFiles();
public:
    DemuxMt4Stream();
    DemuxMt4Stream(string filename);
    void addFiles(string path);
    void addFile(string filename);
    void clearFiles();
    void start();
    void stop();
};
typedef shared_ptr<DemuxMt4Stream> DemuxMt4StreamPtr;

#endif // RAWTICKMT4FILESTREAM_H
