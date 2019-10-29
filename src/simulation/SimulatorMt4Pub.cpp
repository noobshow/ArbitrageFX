#include <sstream>
#include <cstdlib>
#include "simulation/SimulatorMt4Pub.hpp"

static int ticket = 1;

void SimulatorMt4Pub::AsyncUpdate(RequestCommandPtr reqCmdPtr)
{
    switch (reqCmdPtr->command_id_) {
    case CMD_OPEN_ORDER_REQ:
    {
        // received order from ArbitrageCore, simulate open order
        LOG_INFO << "Received Open Order Command: " << reqCmdPtr->to_cmd_string();

        //std::this_thread::sleep_for(std::chrono::milliseconds(500));
        // Response -> <cmd_id>, <order_id>, <arb_id>, <status>, <price>
        int status = (rand() % 3) + 3; // <OR_FILLED = 3, OR_ERROR = 4, OR_TIMEOUT = 5>
        status = 3;
        std::stringstream ss;

        OpenOrderCmdPtr openOrderPtr = reinterpret_cast<OpenOrderCmdPtr&>(reqCmdPtr);

        ss << CMD_OPEN_ORDER_RESP << DELIMITER << openOrderPtr->order_id_ << DELIMITER
           << openOrderPtr->trade_id_ << DELIMITER << ticket++ << DELIMITER << status
           << DELIMITER << openOrderPtr->price_;

        Notify(std::make_shared<std::string>(ss.str()));
        break;
     }
     case CMD_CLOSE_ORDER_REQ:
    {
        // received order from ArbitrageCore, simulate close order
        LOG_INFO << "Received Close Order Command: " << reqCmdPtr->to_cmd_string();
        CloseOrderCmdPtr closeOrderCmdPtr = reinterpret_cast<CloseOrderCmdPtr&>(reqCmdPtr);
        int ticket = closeOrderCmdPtr->ticket_;
        //std::this_thread::sleep_for(std::chrono::milliseconds(500));

        int status = (rand() % 3) + 3; // <OR_FILLED = 3, OR_ERROR = 4, OR_TIMEOUT = 5>
        status = 3;
        std::stringstream ss;

        CloseOrderCmdPtr closeOrderPtr = reinterpret_cast<CloseOrderCmdPtr&>(reqCmdPtr);
        ss << CMD_CLOSE_ORDER_RESP << DELIMITER << closeOrderPtr->order_id_ << DELIMITER
           << closeOrderPtr->trade_id_ << DELIMITER << ticket << DELIMITER << status
           << DELIMITER << closeOrderPtr->price_;

        Notify(std::make_shared<std::string>(ss.str()));
        break;
    }
    default:
        LOG_ERROR << "Invalid CommandId = " << reqCmdPtr->command_id_
                  << ", Cmd: " << reqCmdPtr->to_cmd_string();
        break;
    }

}
