#pragma once


#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <atomic>
#include <optional>

#include "defines.hpp"

namespace tcp {
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
}


class Socket {
    int socket_fd_ = -1; 
    int client_fd_ = -1;

    struct tcp::addrinfo* addrinfo_ = nullptr;

public: 
    ~Socket();

    bool connect(const std::string& port, const std::string& host);

    bool bind(const std::string& port);

    bool listen(int backlog);

    bool accept();

    std::optional<int> send(const std::string& msg);
    std::optional<int> recv();

    void close();

private:
    bool getAddrInfo(const std::string& port, const std::string& host = std::string{});

    bool createSocket(struct tcp::addrinfo* p);
};

