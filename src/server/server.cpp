#include "server.hpp"

Server::Server(const std::string& ip_addr, const std::string& port) 
    : 
        server_info{nullptr},
        ip_address(ip_addr), 
        port(port) 
    {
        init();
    }

Server::~Server() {
    freeaddrinfo(server_info);
    close(socket_fd);
}

void Server::init() {
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status;
    /// заполняем server_info на основе hints
    if ((status = getaddrinfo(NULL, port.c_str(), &hints, &server_info)) != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        std::exit(1);
    }
    
    /// дескриптор сокета
    struct addrinfo * p;
    for (p = server_info; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::perror("server socket error");
            continue;
        }

        int opt = 1;
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
            std::perror("setsockopt error");
            exit(1);
        }

        /// связываем с портом, полученным из getaddrinfo() (bind - для сервера)
        if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_fd);
            std::perror("bind error");
            continue;
        }
        break;
    }

    if (p == NULL) {
        std::cerr << "server: failed to bind\n";
        std::exit(1);
    }

    if (listen(socket_fd, BACKLOG) == -1) {
        std::cerr << "listen error\n";
        std::exit(1);
    }
}

void Server::connect() {
    std::cout << "server: waiting for connections…\n";

    socklen_t calling_size = sizeof(calling_info);    
    listen_fd = accept(socket_fd, (struct sockaddr *)&calling_info, &calling_size);
    
    if (listen_fd == -1) {
        std::cerr << "server accept error\n";
        std::exit(1);
    }
}

void Server::start() {    
    while (is_active) {
        addSession();
    }
}

void Server::stop() {
    is_active = false;
}

void Server::addSession() {
    connect();
    auto session = std::make_unique<ServerSession>(listen_fd);

    std::thread session_thread(&ServerSession::start, session.get());
    
    std::scoped_lock lock(sessions_mtx);
    sessions.emplace_back(std::move(session));

    if (session_thread.joinable()) session_thread.join();
}

std::string Server::getIPaddr() const {
    char buffer[INET_ADDRSTRLEN];
    inet_ntop(
        AF_INET, 
        &(((struct sockaddr_in *)server_info->ai_addr)->sin_addr), 
        buffer, 
        INET_ADDRSTRLEN);
    return buffer;
}