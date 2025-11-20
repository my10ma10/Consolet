#include "chat.hpp"
#include "user.hpp"

#include <algorithm>

Chat::Chat(
    std::shared_ptr<DB> db, 
    std::vector<User>& users, 
    ChatType::Type type,
    const std::optional<std::string>& name)
    : db_(db), name_(name), type_(type)
{
    if (users.empty()) 
        throw std::logic_error("There are no users to add in chat constructor");
    
    if (type == ChatType::Type::PERSONAL && users.size() != 2) 
        throw std::invalid_argument(std::string("Personal chat has no ") + std::to_string(users.size()) + " != 2 users");

    std::for_each(users.begin(), users.end(), [this] (User& user)
    {
        if (user.isSavedToDB()) {
            userIDs_.emplace_back(user.getID());
        }
        else {
            std::cout << "User is not saved in DB" << std::endl;
            db_->save(user);
        }
    });
}

Chat::Chat(
    std::shared_ptr<DB> db, 
    std::vector<ID_t>& userIDs, 
    ChatType::Type type,
    const std::optional<std::string>& name)
    : db_(db), userIDs_(userIDs), name_(name), type_(type)
{
    if (userIDs.empty()) 
        throw std::logic_error("There are no users to add in chat constructor");
    
    if (type == ChatType::Type::PERSONAL && userIDs.size() != 2) 
        throw std::invalid_argument(std::string("Personal chat has no ") + std::to_string(userIDs.size()) + " != 2 users");

    std::for_each(userIDs.begin(), userIDs.end(), [this] (ID_t& userID)
    {
        auto user = db_->findUser(userID);
        if (!user) {
            std::cout << "User is not saved in DB" << std::endl;
            db_->save(*user);
        }
    });
}

void Chat::sendMessage(const std::string& message, ID_t senderId) {
    std::cout << "Message sending: " << message << " to " << senderId << std::endl;
}

void Chat::receiveMessage(const std::string& message, ID_t receiverId) {
    std::cout << "Message receiving: " << message << " from " << receiverId << std::endl;
}

void Chat::addMessage(const std::string& message, ID_t senderId) {
    db_->execute(
        "INSERT INTO MessagesHistory (sender_id, chat_id, text) VALUES (?, ?, ?);",
        senderId, chatID_, message
    );
}

// bool Chat::operator==(const Chat& other) const {
//     return this->chatID_ == other.chatID_ &&
//            this->userIDs_ == other.userIDs_ &&
//            this->name_ == other.name_ &&
//            this->type_ == other.type_;
// }
