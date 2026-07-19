#include "client_session.hpp"

ClientSession::ClientSession(Socket&& socket) 
    : socket_(std::move(socket))
{
}

ClientSession::~ClientSession() {
    is_active_ = false;
}

ClientSession::ClientSession(ClientSession&& other) {
    if (this != &other) {
        user_ = std::move(other.user_);
        is_active_ = other.is_active_.load();
        socket_ = std::move(other.socket_);
        db_ = std::move(other.db_);

        other.is_active_.store(false); 
    }
}

ClientSession& ClientSession::operator=(ClientSession&& other) {
    if (this != &other) {
        stop(); 

        user_ = std::move(other.user_);
        is_active_.store(other.is_active_.load());
        socket_ = std::move(other.socket_);
        db_ = std::move(other.db_);

        other.is_active_.store(false);
    }
    return *this;
}

void ClientSession::start() {
    std::string message;

    std::thread recv_thread([&]() {
        while (is_active_) {
            auto received = socket_.recv();
            // recv json
            if (!received.has_value()) {
                spdlog::debug("Received nullopt: break");
                break;
            }
            if (received->empty()) {
                spdlog::debug("Received empty string: break");
                break;
            }

            // switch (j["cmd"])
            // {
            // case "SEND": {
            //     /* code */
            //     break;
            // }
            // case "SEND": {
            //     /* code */
            //     break;
            // }
            // case "SEND": {
            //     /* code */
            //     break;
            // }
            // case "SEND": {
            //     /* code */
            //     break;
            // }
            
            // default:
            //     break;
            // }
            
            message = received.value();
            spdlog::debug("Received msg: ", message);
        }
    });
        
    std::thread send_thread([&]() {
        while (is_active_) {
            if (message.empty()) {
                spdlog::debug("Entered message is empty: break");
                break;
            }

            if (!is_active_) {
                break;
            }

            socket_.send(message);
        }
    });
    
    if (recv_thread.joinable()) recv_thread.join();
    if (send_thread.joinable()) send_thread.join();
}


void ClientSession::stop() {
    is_active_ = false;
    socket_.close();
}