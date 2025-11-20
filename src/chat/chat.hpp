#pragma once
#include <memory>
#include <vector>
#include <cassert>
#include <string>

#include "db/db.hpp"
#include "chat_type.hpp"

class Chat {
public: 
    friend class DB;

private: 
    std::shared_ptr<DB> db_ = nullptr;
    std::vector<ID_t> userIDs_;
    std::optional<std::string> name_;

    ChatType type_ = ChatType::Type::UNKNOWN;
    ID_t chatID_;

public:
    Chat() = default;

    Chat(
        std::shared_ptr<DB> db,
        std::vector<User>& users,
        ChatType::Type type,
        const std::optional<std::string>& name = std::nullopt
    );
    
    Chat(
        std::shared_ptr<DB> db,
        std::vector<ID_t>& userIDs,
        ChatType::Type type,
        const std::optional<std::string>& name = std::nullopt
    );

    void sendMessage(const std::string& message, ID_t senderId);
    void receiveMessage(const std::string& message, ID_t receiverId);
    
    void addMessage(const std::string& message, ID_t senderId);

    void setID(ID_t newID) { chatID_ = newID; }

    std::string getStringType() const { return type_.toString(); }
    ChatType::Type getType() const { return type_.getType(); }
    std::optional<std::string> getName() const { return name_; }
    ID_t getID() const { return chatID_; }

    bool operator==(const Chat& other) const = default;
};

// class Chat {
//     std::shared_ptr<DB> db_ = nullptr;
//     ID_t chatID_;

// public:
//     Chat() = default;
//     Chat(std::shared_ptr<DB> db);

//     virtual ~Chat() = default;
//     virtual void sendMessage(const std::string& message, ID_t senderId) = 0;
//     virtual void receiveMessage(const std::string& message, ID_t receiverId) = 0;
    
//     void addMessage(const std::string& message, ID_t senderId);

//     virtual std::string getType() const = 0;

//     void setID(ID_t newID) { chatID_ = newID; }
// };


// class PersonalChat : public Chat {
// public:
//     std::pair<ID_t, ID_t> usersID_;
    
// public:
//     PersonalChat() = default;
//     PersonalChat(ID_t uID1, ID_t uID2);
    
//     void sendMessage(const std::string& message, ID_t senderId) override;
//     void receiveMessage(const std::string& message, ID_t receiverId) override;

//     std::string getType() const override { return "personal"; }
// };


// class GroupChat : public Chat {
// public:
//     std::string name_;
//     std::vector<ID_t> usersId_;
    
// public:
//     GroupChat() = default;
//     GroupChat(const std::string& name, std::vector<ID_t> usersId) : usersId_(usersId) {}
    
//     void sendMessage(const std::string& message, ID_t senderId) override;
//     void receiveMessage(const std::string& message, ID_t receiverId) override;

//     std::string getType() const override { return "group"; }
//     std::string getName() const { return name_; }
// };