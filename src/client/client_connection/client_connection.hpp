#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

#include <string.h>
#include <unistd.h>

#include "usr/user.hpp"
#include "db/db.hpp"
#include "message/message.hpp"
#include "chat/chat.hpp"
#include "serializer/serializer.hpp"
#include "tcp_socket/tcp_socket.hpp"

#include <spdlog/spdlog.h>

#include "defines.hpp"


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



