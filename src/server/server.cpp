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
    if (!socket_.bind(port_)) {
        std::exit(1);
    }
    if (!socket_.listen(BACKLOG)) {
        std::exit(1);
    }
    
    while (is_active_) {
        auto listen_socket = socket_.accept();

        auto session = std::make_unique<ClientSession>(std::move(listen_socket.value()));

        std::thread session_thread(&ClientSession::start, session.get());
        
        session_thread.detach();
        {
            std::scoped_lock lock(sessions_mtx_);
            client_sessions_.emplace_back(std::move(session));
        }
    }
}

void Server::stop() {
    is_active_ = false;
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