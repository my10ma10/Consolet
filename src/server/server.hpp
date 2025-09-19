#pragma once
#include <iostream>
#include <cassert>
#include <unistd.h>

#include <netdb.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define BACKLOG 10
#define SIZE 1024

/*
int getaddrinfo(
    const char *node,               // например, "www.example.com" или IP     
    const char *service,            // например, "http" или номер порта
    const struct addrinfo *hints,  
    struct addrinfo **res);
    
    Жизненный цикл сервера:
    socket() -> bind() -> listen() -> accept() -> read/write
*/

class Server {
    struct addrinfo * server_info; // содержит sockaddr
    struct sockaddr_storage calling_info;

    int socket_fd;
    std::string ip_address;
    std::string port;

    std::string message = "pong";
    
    char recv_buf[SIZE];
    int recv_len;
    
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