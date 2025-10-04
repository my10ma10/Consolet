#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <termios.h>

#include "user.hpp"

#define SIZE 4096


class Connection {
    struct addrinfo* client_info;
    int socket_fd;
    std::string ip_address;
    std::string port;

    std::vector<char> recv_buf = std::vector<char>(SIZE);
    int recv_len;
    
    std::string message;
    std::atomic<bool> is_active{true};

public:
    Connection(
        const std::string& server_ip_address, 
        const std::string& server_port
    );
    ~Connection();

    void init();
    void connect();
    void start();
    void stop();

    void recieve();
    void send();

    void printMsg();
};



