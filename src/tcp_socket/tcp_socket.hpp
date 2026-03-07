#pragma once

#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <atomic>
#include <optional>

#include "defines.hpp"


#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

namespace tcp {
    using ::getaddrinfo;
    using ::freeaddrinfo;
    using ::socket;
    using ::bind;
    using ::listen;
    using ::accept;
    using ::send;
    using ::recv;
    using ::setsockopt;
    using ::gai_strerror;
    using ::connect;
    using ::shutdown;
}


class Socket {
    int socket_fd_ = -1; 

    struct addrinfo* addrinfo_ = nullptr;

public: 
    Socket() = default;
    Socket(int fd);
    ~Socket();

    Socket(const Socket& other) = delete;
    Socket& operator=(const Socket& other) = delete;
    
    Socket(Socket&& other);
    Socket& operator=(Socket&& other);

    bool connect(const std::string& port, const std::string& host);

    bool bind(const std::string& port);

    bool listen(int backlog);

    std::optional<Socket> accept();

    std::optional<int> send(const std::string& msg);
    std::optional<int> send(const void* data, const std::size_t size);
    std::optional<std::string> recv();

    void close();

    bool isActive() const;

private:
    void shutdown();
    bool createSocket(struct addrinfo* p);
};

