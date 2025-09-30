#include "session.hpp"

Session::Session(const std::string& ip_address, const std::string& port) {
    client = std::make_shared<Connection>(ip_address, port);
}


void Session::setUser(std::shared_ptr<User> u) {
    user = u;
}

void Session::setConnection(std::shared_ptr<Connection> c) {
    client = c;
}