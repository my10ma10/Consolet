#pragma once
#include <iostream>
#include <vector>
#include <atomic>
#include <mutex>

#include "server_session/server_session.hpp"


class Server {
    std::atomic<bool> is_active{true};
    std::mutex sessions_mtx;
    std::vector<std::unique_ptr<ServerSession> > sessions;
    
    struct addrinfo * server_info; // содержит sockaddr
    struct sockaddr_storage calling_info;

    std::string ip_address;
    std::string port;

    int socket_fd;
    int listen_fd;

public:
    Server(const std::string& ip_addr, const std::string& port);
    ~Server();
    
    void init();
    void connect();

    void start();
    void stop();

    void addSession();


    std::string getIPaddr() const;
};