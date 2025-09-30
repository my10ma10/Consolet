#pragma once
#include <memory>
#include <vector>

#include "user.hpp"
#include "client.hpp"

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