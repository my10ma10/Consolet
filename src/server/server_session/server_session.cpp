#include "server_session.hpp"

ServerSession::ServerSession(int client_fd) 
    : listen_fd(client_fd)
{}

ServerSession::~ServerSession() {
    close(listen_fd);
}

void ServerSession::start() {        
    if (is_active) {
        std::thread recv_thread([&] () {
            while (is_active) {
                recieve();
                if (!is_active) break;
                printMsg();
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

                if (res > 0 && (pfd.revents & POLLIN)) {
                    std::cout << "Enter message to client: \n";
                    std::flush(std::cout);
                    
                    if (std::getline(std::cin, message)) {
                        if (!is_active) break;
                        send();         
                    }
                    
                    if (std::cin.fail() && !std::cin.eof()) {
                        std::cin.clear();
                    }
                }
                
            }
            fcntl(STDIN_FILENO, F_SETFL, cin_flags);
        });
        
        if (recv_thread.joinable()) recv_thread.join();
        if (send_thread.joinable()) send_thread.join();
    }
}

void ServerSession::stop() {
    is_active = false;
    shutdown(listen_fd, SHUT_RDWR);
}

void ServerSession::recieve() {
    std::fill(recv_buf.begin(), recv_buf.end(), 0);
        
    if ((recv_len = ::recv(listen_fd, recv_buf.data(), SIZE, 0)) == -1) {
        std::cerr << "server recv error\n";
        std::flush(std::cerr);
        stop();
        return;
    }
    else if (recv_len == 0) {
        std::cerr << "The connection was closed by client " << listen_fd << std::endl;
        std::flush(std::cerr);
        stop();
        return;
    }
}

void ServerSession::send() {
    if (::send(listen_fd, message.c_str(), strlen(message.c_str()), 0) == -1) {
        std::cerr << "server sending error\n";
        std::flush(std::cerr);

        stop();
        return;
    }
}

void ServerSession::printMsg() {
    std::cout << "server recieved message: ";
    for (size_t i = 0; i < recv_len; ++i) {
        printf("%c", recv_buf[i]);
    }
    printf("\n");
    
    std::flush(std::cout);
}


void ServerSession::setUser(std::unique_ptr<User> u) {
    user = std::move(u);
}