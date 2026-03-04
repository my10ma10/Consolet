#include "server.hpp"

Server::Server(const std::string& ip_addr, const std::string& port) 
: 
    ip_address_(ip_addr), 
    port_(port)
{}

Server::~Server() {
    stop();
}


void Server::start() {  
    if (!listen_socket_.bind(port_)) {
        std::exit(1);
    }
    if (!listen_socket_.listen(BACKLOG)) {
        std::exit(1);
    }
    
    while (is_active_) {
        addSession();
    }
}

void Server::stop() {
    is_active_ = false;
    listen_socket_.close();
}

void Server::addSession() {
    auto client_socket = listen_socket_.accept();

    auto session = std::make_unique<ServerSession>(std::move(*client_socket));

    std::thread session_thread(&ServerSession::start, session.get());
    
    session_thread.detach();
    {
        std::scoped_lock lock(sessions_mtx_);
        sessions_.emplace_back(std::move(session));
    }
}

// std::string Server::getIPaddr() const {
//     char buffer[INET_ADDRSTRLEN];
//     inet_ntop(
//         AF_INET, 
//         &(((struct sockaddr_in *)server_info->ai_addr)->sin_addr), 
//         buffer, 
//         INET_ADDRSTRLEN);
//     return buffer;
// }