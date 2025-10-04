#pragma once
#include <memory>
#include <vector>

#include "user.hpp"
#include "client.hpp"

class ClientSession {
    std::unique_ptr<User> user;
    std::unique_ptr<Connection> client;
    
public:
    ClientSession(
        const std::string& ip_address, 
        const std::string& port
    );

    void auth();
    void start();


    void setUser(std::unique_ptr<User> u);
    void setConnection(std::unique_ptr<Connection> c);
};

void disableEcho();
void enableEcho();