#include "tcp_socket.hpp"

#include <vector>
#include "spdlog/spdlog.h"

Socket::~Socket()
{
    Socket::close();
}

bool Socket::connect(const std::string& port, const std::string& host) {
    if (!getAddrInfo(port, host)) return false;

    struct tcp::addrinfo * p;
    for (p = addrinfo_; p != NULL; p = p->ai_next) {
        if (!createSocket(p)) return false;

        if (tcp::connect(socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
            std::perror("connecting error");
            return false;
        }
    }

    Socket::close();
    return true;
}

bool Socket::bind(const std::string& port) {
    if (!getAddrInfo(port)) return false;

    struct tcp::addrinfo * p;
    for (p = addrinfo_; p != NULL; p = p->ai_next) {
        if (!createSocket(p)) return false;

        int opt = 1;
        if (tcp::setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
            std::perror("setsockopt error");
            return false;
        }

        if (tcp::bind(socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
            std::perror("connecting error");
            return false;
        }
    }

    Socket::close();
    return true;
}

bool Socket::listen(int backlog) {
    if (tcp::listen(socket_fd_, backlog) == -1) {
        std::perror("listen error");
        return false;
    }
    return true;
}

bool Socket::accept() {
    struct tcp::sockaddr_storage client_info;
    socklen_t info_size = sizeof(client_info);    
    int client_fd = tcp::accept(socket_fd_, (struct tcp::sockaddr *)&client_info, &info_size);

    if (client_fd == -1) {
        std::perror("accept error");
        return false;
    }
    return true;
}

std::optional<int> Socket::send(const std::string& msg) {
    if (socket_fd_ < 0) return std::nullopt;

    auto sent = tcp::send(client_fd_, msg.c_str(), msg.size(), 0);
    if (sent == -1) {
        std::perror("send error");
        return std::nullopt;
    }
    return sent;
}

std::optional<int> Socket::recv() {
    if (socket_fd_ < 0) return std::nullopt;

    std::vector<char> recv_buf;
    std::fill(recv_buf.begin(), recv_buf.end(), 0);


    auto received = tcp::recv(client_fd_, recv_buf.data(), BUF_SIZE, 0);
    if (received == -1) {
        std::perror("recv error");
        return std::nullopt;
    }
    else if (received == 0) { 
        spdlog::critical("The connection was closed by client {}", client_fd_);
    }
    return received;
}

void Socket::close() {
    if (socket_fd_ >= 0) {
        ::close(socket_fd_);
        socket_fd_ = -1;
    }
}

bool Socket::getAddrInfo(const std::string& port, const std::string& host) {
    struct tcp::addrinfo hints;

    std::memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = tcp::SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;    
    if ((status = getaddrinfo(host.c_str(), port.c_str(), &hints, &addrinfo_)) != 0) {
        std::cerr << "getaddrinfo error: " << tcp::gai_strerror(status) << std::endl;
        return false;
    }
    return true;
}

bool Socket::createSocket(struct tcp::addrinfo *p) {
    if ((socket_fd_ = tcp::socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        std::perror("server socket error");
        return true;
    }
    return false;
}
