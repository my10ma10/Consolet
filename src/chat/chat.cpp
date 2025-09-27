#include "chat.hpp"

void Chat::addMessage(const std::string& message, std::shared_ptr<User> sender) {
    assert(sender == nullptr && "Sender must not be null");
    assert(!message.empty() && "Message must not be empty");

    messageHistory.emplace_back(std::make_pair(sender, message));
}

void PersonalChat::sendMessage(const std::string& message, std::shared_ptr<User> sender) {
    assert(sender == nullptr && "Sender must not be null");

    if (sender == user1) {
        
        user2->notify();
    }
    else if (sender == user2) {
        user1->notify();
    }
    else {
        exit(1);
    }
}

void PersonalChat::receiveMessage(const std::string &message, std::shared_ptr<User> receiver) {
}
