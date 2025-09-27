#include "client.hpp"


Connection::Connection(const std::string& server_ip_address, const std::string& server_port) 
    : 
        ip_address(server_ip_address), 
        port(server_port)
    {}

Connection::~Connection() {
    freeaddrinfo(client_info);
    close(socket_fd);
}

void Connection::init() {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;


    int status;
    if ((status = getaddrinfo(ip_address.c_str(), port.c_str(), &hints, &client_info)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    
    struct addrinfo * p;
    for (p = client_info; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client socket error");
            continue;
        }

        if (connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connecting error");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "server: failed to connect\n");
        exit(2);
    }

    char other_hand[SIZE];
    inet_ntop(
        p->ai_family, 
        &(((struct sockaddr_in *)p->ai_addr)->sin_addr), 
        other_hand,
        sizeof(other_hand)
    );
    printf("client: connecting to %s\n", other_hand);
}

void Connection::start() {
    init();

    for (;;) {
        std::cout << "Enter message to server: \n";
        std::cin >> message;
        send();
        recieve();
    }
}

void Connection::recieve() {
    // std::cout << "Waiting for message from server..." << std::endl;
    if ((recv_len = recv(socket_fd, &recv_buf, SIZE, 0)) == -1) {
        fprintf(stderr, "client recieve error\n");
        exit(1);
    }

    printf("Client recieved message: %s\n", recv_buf);
    fflush(stdout);
}

void Connection::send() {
    // std::cout << "Enter message to server: \n";
    int sent_len = 0; 
    
    if ((sent_len = ::send(socket_fd, message.c_str(), strlen(message.c_str()), 0)) == -1) {
        fprintf(stderr, "Client sending error\n");
        exit(1);
    }

}
