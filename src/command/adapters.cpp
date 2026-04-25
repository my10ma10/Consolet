#include "adapters.hpp"

#include <string>

#include <ncurses.h>

/*
        === NumberedSendMsgCommand ===
*/

NumberedSendMsgCommand::NumberedSendMsgCommand(ClientSession& s, NumberedCI& nci) 
    : SendMsgCommand(s), ci_(nci)
{}

void NumberedSendMsgCommand::execute() {
    std::string text, receiver_name;

    text.reserve(MESSAGE_MAX_SIZE);
    receiver_name.reserve(CHATNAME_MAX_SIZE);
    
    printw("\nEnter receiver name: \n");
    getstr(receiver_name.data());

    printw("\nEnter message: \n");
    getstr(text.data());

    sendMessage(receiver_name, text);
}


/*
        === SlashedSendMsgCommand ===
*/

SlashedSendMsgCommand::SlashedSendMsgCommand(ClientSession& s, SlashedCI& ci) 
    : SendMsgCommand(s), ci_(ci)
{}

void SlashedSendMsgCommand::setArgs(
    const std::string& username, 
    const std::string& msg
) {
    username_ = username;
    msg_ = msg;
}

void SlashedSendMsgCommand::execute() {
    // обработка аргументов из команды `/msg username "message"`
    auto text = args_.at(0);
    auto receiver_name = args_.at(1); // chat or user

    sendMessage(receiver_name, text);
}
