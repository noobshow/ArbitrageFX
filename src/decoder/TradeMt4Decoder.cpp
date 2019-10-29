#include <boost/algorithm/string.hpp>
#include <cstdlib>
#include "TradeMt4Decoder.hpp"

TradeMt4Decoder::TradeMt4Decoder()
{
}


void TradeMt4Decoder::AsyncUpdate(std::shared_ptr<std::string> cmdStr)
{
    //receive cmd from TradeMt4Pull / Demux / FIX_SERVER(not implemented)
    //LOG_INFO << "Received: " << *cmdStr;

    std::vector<std::string> tokens;
    boost::split(tokens, *cmdStr, boost::is_any_of(DELIMITER));

    if(tokens.size() > 1)
    {
        int id = atoi(tokens[0].c_str());
        bool invCmd = false;

        switch (id)
        {
        case CMD_OPEN_ORDER_RESP:
        case CMD_CLOSE_ORDER_RESP:
            if(tokens.size() == 6){
                OrderRespCmdPtr orderRespCmdPtr =
                        std::make_shared<order_resp_cmd_s>(
                            id, tokens[1], tokens[2], tokens[3], tokens[4], tokens[5]);
                if(orderRespCmdPtr->valid_)
                {
                    Notify(orderRespCmdPtr);
                }
                else invCmd = true;
            }
            else invCmd = true;
            break;
        default:
            LOG_ERROR << "Invalid CMD ID: " << *cmdStr;
            break;
        }
        if(invCmd){
            LOG_ERROR << "Invalid CMD " << id << ": " << *cmdStr;
        }
    }
    else{
        LOG_ERROR << "Invalid tokens parse: " << *cmdStr;
    }


}
