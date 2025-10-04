#include "client.hpp"

Connection::Connection(const std::string& server_ip_address, const std::string& server_port) 
    : 
        ip_address(server_ip_address), 
        port(server_port)
    {  
        init();
    }

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
        std::cerr << "getaddrinfo error: %s\n", gai_strerror(status);
        std::flush(std::cerr);

        stop();
    }
    
    
}

void Connection::connect() {
    struct addrinfo * p;
    for (p = client_info; p != NULL; p = p->ai_next) {
        if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            std::perror("client socket error");
            continue;
        }

        if (::connect(socket_fd, p->ai_addr, p->ai_addrlen) == -1) {
            std::perror("connecting error");
            continue;
        }
        break;
    }

    if (p == NULL) {
        std::cerr << "server: failed to connect\n";
        std::exit(2);
    }

    char server_ip[SIZE];
    inet_ntop(
        p->ai_family, 
        &(((struct sockaddr_in *)p->ai_addr)->sin_addr), 
        server_ip,
        sizeof(server_ip)
    );
    std::cout << "client: connecting to " << server_ip << std::endl;;
}

void Connection::start() {
    connect();

    std::thread send_thread([&] () {
        while (is_active) {
            std::cout << "Enter message to server: \n";
            std::getline(std::cin, message);
            send();
        }
    });

    std::thread recv_thread([&] () {
        while (is_active) {
            recieve();
            printMsg();
        }
    });
        
    if (send_thread.joinable()) send_thread.join();
    if (recv_thread.joinable()) recv_thread.join();
}

void Connection::stop() {
    is_active = false;
}

void Connection::recieve() {
    std::fill(recv_buf.begin(), recv_buf.end(), 0);
    
    if ((recv_len = ::recv(socket_fd, recv_buf.data(), SIZE, 0)) == -1) {
        std::cerr << "client recieve error\n";
        stop();
    }
    else if (recv_len == 0) {
        std::cout << "The connection was closed by server\n";
        stop();
    }
}

void Connection::send() {
    int sent_len = 0; 
    
    if ((sent_len = ::send(socket_fd, message.c_str(), strlen(message.c_str()), 0)) == -1) {
        std::cerr << "Client sending error\n";
        stop();
    }

}

void Connection::printMsg() {
    std::cout << "Client recieved message: ";
    for (size_t i = 0; i < recv_len; ++i) {
        printf("%c", recv_buf[i]);
    }
    printf("\n");
}
