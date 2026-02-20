#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <termios.h>

#include "user.hpp"
#include "db.hpp"
#include "message.hpp"
#include "chat.hpp"
#include "serializer/serializer.hpp"

#define BUF_SIZE 4096

class ClientConnection {
    struct addrinfo* client_info;
    int socket_fd;
    std::string ip_address;
    std::string port;

    std::vector<char> recv_buf = std::vector<char>(BUF_SIZE);
    int recv_len;

    std::atomic<bool> is_active{true};
public:
    ClientConnection() = default;    

    ClientConnection(
        const std::string& server_ip_address, 
        const std::string& server_port
    );
    ~ClientConnection();

    void init();
    void start();
    
    void sendToServer(const Message& message);
    void sendToServer(Message&& message);

private:
    void connect();
    void stop();

    void recieveMessage();

    void printMsg();
};



