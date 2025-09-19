#pragma once
#include <iostream>
#include <numeric>
#include <vector>
#include <memory>

#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

/*
    Жизненный цикл клиента:
    socket() -> connect() -> write/read
*/

#define SIZE 1024


class Client {
    struct addrinfo * client_info;
    int socket_fd;
    std::string ip_address;
    std::string port;

    char recv_buf[SIZE];
    int recv_len;
    
    std::string message;

public:
    Client(const std::string& server_ip_address, const std::string& server_port, const std::string& msg = "ping");
    ~Client();
    void init();
    
    void start();

    void recieve();
    void send();
};






class User {
    std::string _name;
    std::string _passwordHash;

    std::vector<std::shared_ptr<User> > dialogs;
public:
    User() {}
    User(const std::string& name) 
        : _name(name) {}
        
    User(const std::string& name, const std::string& passwordHash) 
        : User(name) {_passwordHash = passwordHash;}

    std::string getPasswordHash() const {return _passwordHash;}
};

std::string hash(std::string str);