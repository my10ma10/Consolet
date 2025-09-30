#pragma once
#include <iostream>
#include <vector>

#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define BACKLOG 10
#define SIZE 1024

class Server {
    struct addrinfo * server_info; // содержит sockaddr
    struct sockaddr_storage calling_info;

    int socket_fd;
    std::string ip_address;
    std::string port;

    std::string message = "pong";
    
    size_t recv_len;
    std::vector<char> recv_buf = std::vector<char>(SIZE);
    
    int listen_fd;

    
public:
    Server(const std::string& ip_addr, const std::string& port);
    ~Server();

    void start();
    void init();

    void recieve();
    void send();

    std::string getIPaddr() const;
};