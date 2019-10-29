#include "DemuxMt4Stream.hpp"
#include <dirent.h>
#include <algorithm>

DemuxMt4Stream::DemuxMt4Stream(){

}
DemuxMt4Stream::DemuxMt4Stream(string filename)
{
    DIR* dir = opendir(filename.c_str());
    if(dir != nullptr){
        closedir(dir);
        this->addFiles(filename);
    }
    else{
        this->addFile(filename);
    }
}


void DemuxMt4Stream::start()
{
    this->working = true;
    this->threadProccessFiles = std::thread(&DemuxMt4Stream::processFiles, this);
    this->threadProccessFiles.detach();
}

void DemuxMt4Stream::stop()
{
    this->working = false;
    if(threadProccessFiles.joinable()){
        threadProccessFiles.join();
    }
}

void DemuxMt4Stream::addFiles(string path)
{
    DIR *dir;
    struct dirent *lsdir;
    dir = opendir(path.c_str());

    if(dir != nullptr){

        while ( ( lsdir = readdir(dir) ) != nullptr )
        {
            string filename(path);
            if(path[path.size()-1] != '/') filename.append("/");
            filename.append(lsdir->d_name);
            if(filename.size() > (path.size()+3)){
                files.push_back(filename);
            }
        }
        closedir(dir);
    }
    else{
        LOG_ERROR << "DemuxMt4Stream::addFiles: Diretório inválido: " << path ;
    }
}

void DemuxMt4Stream::addFile(string filename)
{
    files.push_back(filename);
}

void DemuxMt4Stream::clearFiles()
{
    files.clear();
}

void DemuxMt4Stream::processFiles()
{
    try{
        std::string line;
        std::sort(this->files.begin(), this->files.end());

        for (std::vector<string>::iterator it = this->files.begin() ; it != this->files.end(); ++it)
        {
            ifstream file( *it );

            if (file.is_open())
            {
                while (std::getline(file, line))
                {
                    std::size_t found = line.find(",");
                    if (found != std::string::npos)
                    {
                        int id = atoi(line.substr(0, found).c_str());
                        if(id == CMD_PRICE)
                        {
                            Notify(router::in_price, std::make_shared<std::string>(line));
                        }
                        /*else if( id % 2 == 1)
                        {
                            Notify(router::in_trade, std::make_shared<std::string>(line));
                        }
                        else
                        {
                            Notify(router::out_trade, std::make_shared<std::string>(line));
                        }*/
                    }
                    else{
                        LOG_ERROR << "Não foi possivel encontrar ID da mensagem: " << line;
                    }
                }
                file.close();

                //LOG_INFO << "Stream de arquivo carregado com sucesso, nome: " << *it ;
            }
            else{
                LOG_ERROR << "Erro ao abrir Stream de arquivo, nome: " << *it ;
            }
        }

    }
    catch (exception& e)
    {
        LOG_ERROR << "DemuxMt4Stream::loadFiles: Exception " << e.what() ;
    }
    this->working = false;
}



