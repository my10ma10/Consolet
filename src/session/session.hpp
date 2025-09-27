#pragma once
#include <memory>

#include "../usr/user.hpp"
#include "../client/client.hpp"

class Session {
    std::shared_ptr<User> user;
    std::shared_ptr<Connection> client;

public:
    Session(
        const std::string& ip_address, 
        const std::string& port
        );
    
    void start();

    void setUser(std::shared_ptr<User> u);
    void setConnection(std::shared_ptr<Connection> c);
};