#pragma once
#include <memory>
#include <vector>
#include <cassert>

#include "user.hpp"

class Chat {
protected:
    std::shared_ptr<User> user1;
    std::shared_ptr<User> user2;
    
    std::vector<std::pair<std::shared_ptr<User>, std::string>> messageHistory;

    bool _unread;
public:
    Chat(std::shared_ptr<User> u1, std::shared_ptr<User> u2) 
        : user1(u1), user2(u2), _unread(false)
        {}

    virtual ~Chat() = default;
    virtual void sendMessage(const std::string& message, std::shared_ptr<User> sender) = 0;
    virtual void receiveMessage(const std::string& message, std::shared_ptr<User> receiver) = 0;
    
    /// @brief Add message in the `messageHistory`
    void addMessage(const std::string& message, std::shared_ptr<User> sender);
};


class PersonalChat : public Chat {
public:
    PersonalChat(std::shared_ptr<User> u1, std::shared_ptr<User> u2) 
        : Chat(u1, u2) 
        {}
    
    void sendMessage(const std::string& message, std::shared_ptr<User> sender) override;
    void receiveMessage(const std::string& message, std::shared_ptr<User> receiver) override;
};
