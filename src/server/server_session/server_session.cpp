// #include "server_session.hpp"
// #include "spdlog/spdlog.h"

// ServerSession::ServerSession(int client_fd) 
//     : listen_socket_(client_fd)
// {}

// ServerSession::ServerSession(Socket&& socket)
//     : listen_socket_(std::move(socket))
// {
// }

// ServerSession::~ServerSession() {
//     is_active = false;
// }

// void ServerSession::start() {        
    
// }

// void ServerSession::stop() {
//     is_active = false;
//     listen_socket_.close();
// }

// void ServerSession::setUser(std::unique_ptr<User> u) {
//     user = std::move(u);
// }