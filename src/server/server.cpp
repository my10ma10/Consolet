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

    std::unique_ptr<ClientSession> session = Server::accept();

    ThreadPool pool;
    
    while (true) {
        while (is_active_) {
            pool.enqueue([session = std::move(session), this, &pool]() mutable {
                std::thread session_thread([session = std::move(session)]() mutable {
                    session->start();
                });

                
                
                session_thread.detach();
                {
                    std::scoped_lock lock(sessions_mtx_);
                    client_sessions_.emplace_back(std::move(session));
                }
            });
        }
    }
}

void Server::stop() {
    is_active_ = false;
}

std::unique_ptr<ClientSession> Server::accept() {
    return std::make_unique<ClientSession>(
        std::move(socket_.accept().value_or(Socket{}))
    );
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