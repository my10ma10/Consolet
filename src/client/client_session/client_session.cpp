#include "client_session.hpp"

ClientSession::ClientSession(const std::string& ip_address, const std::string& port) {
    client = std::make_unique<Connection>(ip_address, port);
}

void ClientSession::auth() {
    std::string login;
    std::string password;

    std::cout << "Enter login: \n";
    std::getline(std::cin, login);

    disableEcho();
    std::cout << "Enter password: \n";
    std::getline(std::cin, password);
    enableEcho();
}


void ClientSession::start() {
    client->start();
}


void ClientSession::setUser(std::unique_ptr<User> u) {
    user = std::move(u);
}

void ClientSession::setConnection(std::unique_ptr<Connection> c) {
    client = std::move(c);
}


void disableEcho() {
    struct termios term;
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void enableEcho() {
    struct termios term; 
    tcgetattr(STDIN_FILENO, &term);
    term.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
}
