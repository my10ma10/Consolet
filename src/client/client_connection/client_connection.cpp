#include "client_connection.hpp"
#include "client.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

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

Message ClientConnection::recv() {
    auto ser_str = socket_.recv();

    if (!ser_str.has_value()) {
        spdlog::error("ClientConnection: recv error");
        throw std::runtime_error("ClientConnection: recv error");
    }

    spdlog::info("Recv msg: {}", ser_str.value());

    return Serializer::deserialize(ser_str.value());
}

void ClientConnection::sendToServer(const Message& message) {
    auto ser_str = Serializer::serialize(message);

    if (socket_.send(ser_str) < 0) {
        spdlog::warn("ClientConnection send error");
        return;
    }

    spdlog::info("Sent msg: {}", ser_str);
}

void ClientConnection::sendToServer(Message&& message) {
    sendToServer(message);
}

bool ClientConnection::recvAuthStatus() {
    try {
        auto j_str = socket_.recv();

        if (!j_str.has_value()) {
            spdlog::error("ClientConnection: recv error");
            throw std::runtime_error("ClientConnection: recv error");
        }

        json j = json::parse(std::move(j_str).value());
        return j.at("auth_status").get<bool>();
    }
    catch (const json::parse_error& e) {
        spdlog::error("JSON parse error (byte {}): {}", e.byte, e.what());
        throw std::runtime_error("Invalid JSON format in auth status");
    }
}

void ClientConnection::printMsg(const std::string& msg) {
    spdlog::info("Client recieved message: '{}'", msg);
}
