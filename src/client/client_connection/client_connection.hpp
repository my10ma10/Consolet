#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

#include <string.h>
#include <unistd.h>

#include "user.hpp"
#include "db.hpp"
#include "message.hpp"
#include "chat.hpp"
#include "serializer/serializer.hpp"

#include "spdlog/spdlog.h"

#include "defines.hpp"
#include "tcp_socket/tcp_socket.hpp"


class ClientConnection {
    std::string ip_address_;
    std::string port_;

    Socket socket_;

    // std::vector<char> recv_buf = std::vector<char>(BUF_SIZE);
    // int recv_len;

    std::atomic<bool> is_active_{true};
public:
    ClientConnection() = default;    

    ClientConnection(
        const std::string& server_ip_address, 
        const std::string& server_port
    );
    ~ClientConnection();

    void start();
    
    void sendToServer(const Message& message);
    void sendToServer(Message&& message);

    void stop();

private:
    std::string recv();

    void printMsg(const std::string& msg);
};



