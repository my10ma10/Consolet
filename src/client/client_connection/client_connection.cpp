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

            if (!std::getline(std::cin, str)) {
                spdlog::debug("getline error");
            }
            
            
            if (str.empty()) {
                spdlog::debug("Entered message is empty: break");
                break;
            }

            socket_.send(str);
            
            if (!is_active_) {
                break;
            }

            
            std::cout.flush();

            if (std::cin.fail() && !std::cin.eof()) {
                std::cin.clear();
            }
        }
    });

    std::thread recv_thread([&] () {
        while (is_active_) {

            auto msg = socket_.recv();
            if (!msg) {
                spdlog::debug("Received nullopt");
            }
            if (msg->empty()) {
                spdlog::debug("Received empty string");
            }
            printMsg(*msg);
            
            if (!is_active_) {
                break;
            }
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
    std::cout << "Client recieved message: '" << msg << "'\n";
    
}
