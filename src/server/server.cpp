#include "server.hpp"

Server::Server(const std::string& ip_addr, const std::string& port) 
    : 
        server_info{nullptr},
        ip_address(ip_addr), 
        port(port) 
    {}

Server::~Server() {
    freeaddrinfo(server_info);
    close(listen_fd);
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
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    
    /// дескриптор сокета
    struct addrinfo * p;
    for (p = server_info; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server socket error");
            continue;
        }

        int yes = 1;

        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt error");
            exit(1);
        }

        /// связываем с портом, полученным из getaddrinfo() (bind - для сервера)
        if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_fd);
            perror("bind error");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(2);
    }

    if (listen(socket_fd, BACKLOG) == -1) {
        fprintf(stderr, "listen error\n");
        exit(1);
    }

    printf("server: waiting for connections…\n"); 
    fflush(stdout);
}

void Server::start() {
    init();

    for (;;) {
        std::cout << "Enter message to client: \n";
        std::cin >> message;
        recieve();
        send();
    }
}

void Server::recieve() {
    // std::cout << "Waiting for message from client..." << std::endl;
    socklen_t calling_size = sizeof(calling_info);
    
    listen_fd = accept(socket_fd, (struct sockaddr *)&calling_info, &calling_size);
    
    if (listen_fd == -1) {
        fprintf(stderr, "server accept error\n");
        exit(1);
    }
    
    if ((recv_len = recv(listen_fd, &recv_buf, SIZE, 0)) == -1) {
        fprintf(stderr, "server recv error\n");
        exit(1);
    }
    else if (recv_len == 0) {
        fprintf(stdout, "The connection was closed by removed hand\n");
    }

    printf("Server recieved message: %s\n", recv_buf);
    fflush(stdout);
}

void Server::send() {
    // std::cout << "Enter message to client: \n";
    if (::send(listen_fd, message.c_str(), strlen(message.c_str()), 0) == -1) {
        fprintf(stderr, "server sending error\n");
        exit(1);
    }
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