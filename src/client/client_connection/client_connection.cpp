#include "client_connection.hpp"
#include "client.hpp"


ClientConnection::ClientConnection(const std::string& server_ip_address, const std::string& server_port) 
    : 
    ip_address_(server_ip_address), 
    port_(server_port)
{}

ClientConnection::~ClientConnection() {
    stop();
}


void ClientConnection::start() {
    if (!socket_.connect(port_, ip_address_)) std::exit(1);

    std::thread send_thread([&] () {
        while (is_active_) {
            std::string str;

            std::cout << "Send msg: ";
            std::cin >> str;
            socket_.send(str);
        }
    });

    std::thread recv_thread([&] () {
        while (is_active_) {
            auto msg = recv();
            printMsg(msg);
        }
    });
        
    if (send_thread.joinable()) send_thread.join();
    if (recv_thread.joinable()) recv_thread.join();
}

void ClientConnection::stop() {
    is_active_ = false;
}

std::string ClientConnection::recv() {
    return socket_.recv().value();
}

void ClientConnection::sendToServer(const Message& message) {
    auto serialized_msg = Serializer::serialize(message);

    if (socket_.send(message.getText()) < 0) {
        std::perror("connection send error");
        return;
    }
}

void ClientConnection::sendToServer(Message&& message) {
    sendToServer(message);
}

void ClientConnection::printMsg(const std::string& msg) {
    std::cout << "Client recieved message: " << msg;
    
}
