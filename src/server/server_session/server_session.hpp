#pragma once
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>

#include <unistd.h>
#include <poll.h>
#include <fcntl.h>

#include "user/user.hpp"
#include "tcp_socket/tcp_socket.hpp"

class ServerSession {
    std::unique_ptr<User> user;
    std::atomic<bool> is_active{true};

    Socket listen_socket_;

    std::string message_;
    
public:
    ServerSession(int client_fd);
    ServerSession(Socket&& socket);
    ~ServerSession();
    
    void start();
    void stop();

    void setUser(std::unique_ptr<User> u);
};
