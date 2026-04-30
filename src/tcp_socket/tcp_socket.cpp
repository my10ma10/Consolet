#include "tcp_socket.hpp"

#include <vector>
#include "spdlog/spdlog.h"

Socket::Socket(int fd) : socket_fd_(fd)
{
    std::memset(&addrinfo_, 0, sizeof(addrinfo_));
}

Socket::~Socket()
{
    Socket::close();
}

Socket::Socket(Socket&& other) {
    if (!other.isActive()) return;
    
    if (this != &other) {
        socket_fd_ = other.socket_fd_;
        addrinfo_ = other.addrinfo_;

        other.socket_fd_ = -1;
        other.addrinfo_ = nullptr;
    }
}

Socket& Socket::operator=(Socket&& other) {
    if (this != &other) {
        this->close();

        socket_fd_ = other.socket_fd_;
        addrinfo_ = other.addrinfo_;
        
        other.socket_fd_ = -1;
        other.addrinfo_ = nullptr;
    }
    return *this;
}

bool Socket::connect(const std::string& port, const std::string& host) {
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host.c_str(), port.c_str(), &hints, &addrinfo_);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << tcp::gai_strerror(status) << std::endl;
        return false;
    }

    struct addrinfo * p;
    for (p = addrinfo_; p != NULL; p = p->ai_next) {
        if (!createSocket(p)) return false;

        if (tcp::connect(socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
            std::perror("connecting error");
            return false;
        }
    }
    spdlog::debug("Connected");
    return true;
}

bool Socket::bind(const std::string& port) {
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = tcp::getaddrinfo(NULL, port.c_str(), &hints, &addrinfo_);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << tcp::gai_strerror(status) << std::endl;
        return false;
    }

    struct addrinfo * p;
    for (p = addrinfo_; p != NULL; p = p->ai_next) {
        if (!createSocket(p)) continue;

        int opt = 1; 
        if (tcp::setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
            std::perror("setsockopt error");
            return false;
        }

        if (tcp::bind(socket_fd_, p->ai_addr, p->ai_addrlen) == -1) {
            std::perror("connecting error");
            return false;
        }
        break;
    }
    spdlog::debug("Binded");

    return true;
}

bool Socket::listen(int backlog) {
    if (tcp::listen(socket_fd_, backlog) == -1) {
        std::perror("listen error");
        return false;
    }
    spdlog::debug("Listened");
    return true;
}

std::optional<Socket> Socket::accept() {
    struct sockaddr_storage client_info;
    socklen_t info_size = sizeof(client_info);    
    int client_fd = tcp::accept(socket_fd_, (struct sockaddr *)&client_info, &info_size);

    if (client_fd == -1) {
        std::perror("accept error");
        return std::nullopt;
    }
    spdlog::debug("Accepted");
    return Socket(client_fd);
}

std::optional<int> Socket::send(const std::string& msg) {
    return Socket::send(msg.c_str(), msg.size());
}

std::optional<int> Socket::send(const void* data, const std::size_t size) {
    if (!isActive()) return std::nullopt;

    auto sent = tcp::send(socket_fd_, data, size, 0);
    if (sent == -1) {
        std::perror("socket send error");
        return std::nullopt;
    }
    return sent;
}

std::optional<std::string> Socket::recv() {
    if (!isActive()) return std::nullopt;

    std::vector<char> recv_buf(BUF_SIZE, 0);
    // std::fill(recv_buf.begin(), recv_buf.end(), 0);


    auto received = tcp::recv(socket_fd_, recv_buf.data(), BUF_SIZE, 0);
    if (received == -1) {
        std::perror("recv error");
        return std::nullopt;
    }
    else if (received == 0) { 
        spdlog::warn("The connection closed by client {}", socket_fd_);
        Socket::shutdown();
    }
    auto recv_str = std::string(recv_buf.begin(), recv_buf.end());

    spdlog::debug("Recieved: {}", recv_str);
    return recv_str;
}

void Socket::close() {
    if (addrinfo_) {
        freeaddrinfo(addrinfo_);
        addrinfo_ = nullptr;
    }
    if (isActive()) {
        Socket::shutdown();
        ::close(socket_fd_);
        socket_fd_ = -1;
    }
}

void Socket::shutdown() {
    if (!isActive()) {
        std::perror("Trying to shutdown empry fd");
        return;
    }
    if (tcp::shutdown(socket_fd_, SHUT_RDWR) == -1) {
        std::perror("Socket shutdown error");
        return;
    }

    socket_fd_ = -1;
}

bool Socket::isActive() const {
    return socket_fd_ >= 0;
}

bool Socket::createSocket(struct addrinfo *p) {
    if ((socket_fd_ = tcp::socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
        std::perror("server socket error");
        return false;
    }
    return true;
}
