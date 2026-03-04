#include "server_session.hpp"

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
        while (is_active) { // is_active не зависит от 
            listen_socket_.recv();
            if (!is_active) break;
        }
    });
        
    std::thread send_thread([&] () {
        int cin_flags = fcntl(STDIN_FILENO, F_GETFL); // get currenct cin status
        fcntl(STDIN_FILENO, F_SETFL, cin_flags | O_NONBLOCK); // set nonblock status
        
        while (is_active) {
            struct pollfd pfd;
            pfd.fd = STDIN_FILENO;
            pfd.events = POLLIN;

            int res = poll(&pfd, 1, 100);

            if (res <= 0 || !(pfd.revents & POLLIN)) {
                continue;
            }
            std::cout << "Enter message to client: \n";
            
            if (std::getline(std::cin, message_)) {
                if (!is_active) break;

                listen_socket_.send(message_);         
            }
            
            if (std::cin.fail() && !std::cin.eof()) {
                std::cin.clear();
            }
            
        }
        fcntl(STDIN_FILENO, F_SETFL, cin_flags);
    });
    
    if (recv_thread.joinable()) recv_thread.join();
    if (send_thread.joinable()) send_thread.join();
}

void ServerSession::stop() {
    is_active = false;
    listen_socket_.shutdown();
}

void ServerSession::setUser(std::unique_ptr<User> u) {
    user = std::move(u);
}