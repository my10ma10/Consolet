#include "client.hpp"
#include "command/command.hpp"
#include "command/command_factory.hpp"

#include <ncurses.h>

Client::Client(const std::string& ip_address, const std::string& port) 
{
    localDB_ = std::make_shared<ClientCacheDB>();

    if (localDB_) {
        localDB_->init("local.db", std::string(PROJECT_SOURCE_DIR) + std::string(Defines::CLIENT_CACHE_DB_PATH));
        connection_ = std::make_unique<ClientConnection>(ip_address, port);
    }
}

void Client::auth() {
    std::string nickname, password;

    std::cout << "Welcome to first version of Consolet authentificator\n"
                 "Test session nickname: 'nickname'\n"
                 "Test session password: '123'\n";

    std::cout << "Enter nickname: \n";
    std::getline(std::cin, nickname);

    disableEcho();
    std::cout << "Enter password: \n";
    std::getline(std::cin, password);
    enableEcho();

    // TODO: request authentification on server, enable password hash 
    // if (authStatus()) {
    //     std::cout << "Success authentification!" << std::endl;
    // }
    // else {
    //     spdlog::warn("Invalid nickname or password");
    // }
}


void Client::start() {
    // TODO: have to get client ID from server

    std::cout << getHelpMsg() << std::endl;

    auto commands = selectUI();

    printw("%s\n", cli_->help().c_str());
    cli_->display(commands);
    
    
    if ([[maybe_unused]] auto* tmp = dynamic_cast<NumberedCI*>(cli_.get())) {
        cli_->processChoice(commands, static_cast<std::size_t>(getch() - '0'));
    } 
    else if ([[maybe_unused]] auto* tmp = dynamic_cast<SlashedCI*>(cli_.get())) {
        std::string command_str(1024, 0);
        getstr(command_str.data());
        
        command_str.erase(command_str.find_last_not_of(" \n\t\r") + 1);

        cli_->processChoice(commands, command_str.data());
    }
    else {
        throw std::logic_error("Unknown type: bad dynamic_cast from CommandInterface");
    }
    getch();
    
}

void Client::sendToServer(Message&& message) const {
    connection_->sendToServer(message);
}

std::string Client::getHelpMsg() const {
    return std::string("Welcome to Consolet!\n"
            "You are able to use numbered interface:\n"
            "   1. Send message\n"
            "   2. Open chat\n"
            "   3. Print chats list\n"
            "   0. Exit\n\n"
            "Also you can use command interface, e.g.\n"
            "   /list - print the list of the active chats\n"
            "   /msg receiver message - send message to receiver (chat name or user name)\n"
        );
}

void Client::setConnection(std::unique_ptr<ClientConnection> c) {
    connection_ = std::move(c);
}

bool Client::authStatus() {
    return connection_->recvAuthStatus();
}

std::vector<std::unique_ptr<ICommand>> Client::selectUI() {
    std::size_t ui_choice = 0;
    std::cout << "Select CLI:\n1. Numbered CLI\n2. Slashed CLI\n";

    std::cin >> ui_choice;
    std::cin.ignore();

    // TODO: перейти к принципу открытости-закрытости
    if (ui_choice == 1) {
        cli_ = std::make_unique<NumberedCI>();

        return CommandFactory::createForNumberedCI(
            *this, 
            dynamic_cast<NumberedCI&>(*cli_)
        );
    }
    else if (ui_choice == 2) {
        cli_ = std::make_unique<SlashedCI>();

        return CommandFactory::createForSlashedCI(
            *this, 
            dynamic_cast<SlashedCI&>(*cli_)
        );
    }
    else if (ui_choice == 3) {
        connection_->start();
    }
    spdlog::error("Unexpected CLI choice");
    throw std::invalid_argument("Unexpected CLI choice");
}
