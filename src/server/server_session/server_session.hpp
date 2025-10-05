#pragma once
#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include <mutex>

#include <unistd.h>
#include <netdb.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>

#include "user.hpp"


#define BACKLOG 10
#define SIZE 4096

/// @brief The connection of the client in the server
class ServerSession {
    std::unique_ptr<User> user;
    std::atomic<bool> is_active{true};

    int listen_fd;

    std::string message;
    
    size_t recv_len;
    std::vector<char> recv_buf = std::vector<char>(SIZE);
    
public:
    ServerSession(int client_fd);
    ~ServerSession();

    
    void start();
    void stop();

    void recieve();
    void send();

    void printMsg();

    void setUser(std::unique_ptr<User> u);
};
