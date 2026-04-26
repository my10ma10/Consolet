#pragma once

#include "command.hpp"


#define CHATNAME_MAX_SIZE 256
#define MESSAGE_MAX_SIZE 4096

class NumberedSendMsgCommand : public SendMsgCommand {
    NumberedCI& ci_;

public:
    NumberedSendMsgCommand(Client& s, NumberedCI& nci);
    
    void execute() override;

};

class SlashedSendMsgCommand : public SendMsgCommand {
    SlashedCI& ci_;

    std::string username_;
    std::string msg_;

public:
    SlashedSendMsgCommand(Client& s, SlashedCI& ci);
    void setArgs(const std::string& username, const std::string& msg);

    void execute() override;
};