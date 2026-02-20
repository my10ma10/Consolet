#include "client_session.hpp"

ClientSession::ClientSession(const std::string& ip_address, const std::string& port) 
{
    localDB_ = std::make_shared<ClientCacheDB>();

    if (localDB_) {
        localDB_->init("local.db", std::string(PROJECT_SOURCE_DIR) + "/assets/sql/create_client_cache_DB.sql");
        connection_ = std::make_unique<ClientConnection>(ip_address, port);
    }
}

void ClientSession::auth() {
    std::string login;
    std::string password;

    std::cout << "Welcome to first version of Consolet authentificator\n"
                 "Test session login: 'login'\n"
                 "Test session password: '123'\n";

    std::cout << "Enter login: \n";
    std::getline(std::cin, login);

    disableEcho();
    std::cout << "Enter password: \n";
    std::getline(std::cin, password);
    enableEcho();
}


void ClientSession::start() {
    //! have to get client ID from server

    std::cout << getHelpMsg() << std::endl;

    auto commands = selectUI();

    printw("%s\n", ci_->help().c_str());
    ci_->display(commands);
    
    
    if ([[maybe_unused]] auto* tmp = dynamic_cast<NumberedCI*>(ci_.get())) {
        ci_->processChoice(commands, static_cast<std::size_t>(getch() - '0'));
    } 
    else if ([[maybe_unused]] auto* tmp = dynamic_cast<SlashedCI*>(ci_.get())) {
        std::string command_str(1024, 0);
        getstr(command_str.data());
        
        command_str.erase(command_str.find_last_not_of(" \n\t\r") + 1);

        ci_->processChoice(commands, command_str.data());
    }
    else {
        throw std::logic_error("Unknown type: bad dynamic_cast from CommandInterface");
    }
    getch();
    // connection_->start();
    
}

void ClientSession::sendToServer(Message&& message) const {
    connection_->sendToServer(message);
}

std::string ClientSession::getHelpMsg() const
{
    return  std::string("Welcome to Consolet!\n"
            "You are able to use numbered interface:\n"
            "   1. Написать сообщение\n"
            "   2. Открыть чат\n"
            "   3. Вывести список чатов\n"
            "   0. Выход\n\n"
            "Also you can use command interface, e.g.\n"
            "   /list - print the list of the active chats\n"
            "   /msg receiver message - send message to receiver (chat name or user name)\n"
        );
}

void ClientSession::setUser(std::unique_ptr<User> u) {
    user_ = std::move(u);
}

void ClientSession::setConnection(std::unique_ptr<ClientConnection> c) {
    connection_ = std::move(c);
}

std::vector<std::unique_ptr<ICommand>> ClientSession::selectUI() {
    std::size_t ui_choice = 0;
    std::cout << "Select UI:\n1. Numbered CI\n2. Slashed CI\n";

    std::cin >> ui_choice;
    std::cin.ignore();

    if (ui_choice == 1) {
        ci_ = std::make_unique<NumberedCI>();

        return CommandFactory::createForNumberedCI(
            *this, 
            dynamic_cast<NumberedCI&>(*ci_)
        );
    }
    else if (ui_choice == 2) {
        ci_ = std::make_unique<SlashedCI>();

        return CommandFactory::createForSlashedCI(
            *this, 
            dynamic_cast<SlashedCI&>(*ci_)
        );
    }
    else {
        throw std::invalid_argument("Unexpected CI choice");
    }
}
