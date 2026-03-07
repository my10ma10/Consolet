#include "server_session.hpp"
#include "spdlog/spdlog.h"

ServerSession::ServerSession(int client_fd) 
    : listen_socket_(client_fd)
{}

ServerSession::ServerSession(Socket&& socket)
    : listen_socket_(std::move(socket))
{
}

ServerSession::~ServerSession() {
    is_active = false;
}

void ServerSession::start() {        
    if (!is_active) return;

    std::thread recv_thread([&] () {
        
        while (is_active) {
            std::cout.flush();
            auto received = listen_socket_.recv();
            if (!received) {
                spdlog::debug("Received nullopt: break");
                break;
            }
            if (received->empty()) {
                spdlog::debug("Received empty string: break");
                break;
            }
            message_ = received.value();
        }
    });
        
    std::thread send_thread([&] () {
        while (is_active) {
            if (message_.empty()) {
                spdlog::debug("Entered message is empty: break");
                break;
            }

            if (!is_active) {
                break;
            }

            listen_socket_.send(message_);

        }
    });
    
    if (recv_thread.joinable()) recv_thread.join();
    if (send_thread.joinable()) send_thread.join();
}

void ServerSession::stop() {
    is_active = false;
    listen_socket_.close();
}

void ServerSession::setUser(std::unique_ptr<User> u) {
    user = std::move(u);
}