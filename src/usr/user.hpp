#pragma once
#include <iostream>

#include "../chat/chat.hpp"

class Chat;

class User {
protected:
     std::string _nickname;
     std::string _passwordHash;
     
     std::vector<std::shared_ptr<Chat> > chats;
     unsigned int _notificationCount;
public:
    User() = default;
    User(const std::string& name, const std::string& password)
        : _nickname(name), _passwordHash(password) {}

    void notify();

    std::string getName() const { return _nickname; }
    std::string getPassword() const { return _passwordHash; }
};