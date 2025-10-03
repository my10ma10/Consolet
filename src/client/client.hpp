#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>

#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "user.hpp"

/*
    Жизненный цикл клиента:
    socket() -> connect() -> write/read
*/

#define SIZE 4096


class Connection {
    struct addrinfo * client_info;
    int socket_fd;
    std::string ip_address;
    std::string port;

    std::vector<char> recv_buf = std::vector<char>(SIZE);
    int recv_len;
    
    std::string message;
    std::mutex outMtx;
public:
    Connection(
        const std::string& server_ip_address, 
        const std::string& server_port
    );
    ~Connection();
    void init();
    void connect();
    void start();

    void recieve();
    void send();

    void printMsg();
};



