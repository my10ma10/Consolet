#pragma once
#include <iostream>
#include <vector>
#include <atomic>
#include <mutex>
#include <memory>

#include "server_session/server_session.hpp"
#include "tcp_socket/tcp_socket.hpp"
#include "defines.hpp"


class Server {
    std::atomic<bool> is_active_{true};
    std::mutex sessions_mtx_;
    std::vector<std::unique_ptr<ServerSession>> sessions_;
    
    std::string ip_address_;
    std::string port_;

    Socket listen_socket_;

public:
    Server(const std::string& ip_addr, const std::string& port);
    ~Server();
    
    void start();
    void stop();

    void addSession();

};