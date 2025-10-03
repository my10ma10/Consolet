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
    
    
}

void Connection::connect() {
    struct addrinfo * p;
    for (p = client_info; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client socket error");
            continue;
        }

        if (::connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connecting error");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "server: failed to connect\n");
        exit(2);
    }

    char server_ip[SIZE];
    inet_ntop(
        p->ai_family, 
        &(((struct sockaddr_in *)p->ai_addr)->sin_addr), 
        server_ip,
        sizeof(server_ip)
    );
    printf("client: connecting to %s\n", server_ip);
}

void Connection::start() {
    init();
    connect();

    std::thread send_thread([&] () {
        for (;;) {
            std::cout << "Enter message to server: \n";
            std::getline(std::cin, message);
            send();
        }
    });

    std::thread recv_thread([&] () {
        for (;;) {
            recieve();
            printMsg();
        }
    });
        
    if (send_thread.joinable()) send_thread.join();
    if (recv_thread.joinable()) recv_thread.join();
}

void Connection::recieve() {
    std::fill(recv_buf.begin(), recv_buf.end(), 0);
    
    if ((recv_len = ::recv(socket_fd, recv_buf.data(), SIZE, 0)) == -1) {
        fprintf(stderr, "client recieve error\n");
        exit(1);
    }
    else if (recv_len == 0) {
        fprintf(stdout, "The connection was closed by removed hand\n");
        exit(0);
    }
}

void Connection::send() {
    int sent_len = 0; 
    
    if ((sent_len = ::send(socket_fd, message.c_str(), strlen(message.c_str()), 0)) == -1) {
        fprintf(stderr, "Client sending error\n");
        exit(1);
    }

}

void Connection::printMsg() {
    std::scoped_lock lock(outMtx);
    printf("Client recieved message: \n");
    for (size_t i = 0; i < recv_len; ++i) {
        printf("%c", recv_buf[i]);
    }
    printf("\n");
}
