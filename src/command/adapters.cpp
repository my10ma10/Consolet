#include "adapters.hpp"

#include <string>

#include <ncurses.h>

/*
        === NumberedSendMsgCommand ===
*/

NumberedSendMsgCommand::NumberedSendMsgCommand(Client& s, NumberedCI& nci) 
    : SendMsgCommand(s), ci_(nci)
{}

void NumberedSendMsgCommand::execute() {
    std::string text, receiver_name;

    text.resize(MESSAGE_MAX_SIZE);
    receiver_name.resize(CHATNAME_MAX_SIZE);
    
    printw("\nEnter receiver name: \n");
    getnstr(receiver_name.data(), MESSAGE_MAX_SIZE - 1);

    printw("\nEnter message: \n");
    getnstr(text.data(), CHATNAME_MAX_SIZE - 1);

    receiver_name.resize(std::strlen(receiver_name.c_str()));
    text.resize(std::strlen(text.c_str()));

    sendMessage(receiver_name, text);
}


/*
        === SlashedSendMsgCommand ===
*/

SlashedSendMsgCommand::SlashedSendMsgCommand(Client& s, SlashedCI& ci) 
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
