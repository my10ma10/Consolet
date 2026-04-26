#pragma once
#include <iostream>
#include <optional>

#include "db/db.hpp"

class User {
protected:
    std::string nickname_;
    std::string passwordHash_;
    
    std::optional<ID_t> id_ = std::nullopt;

    size_t notificationCount_ = 0;
public:
    User() = default;
    User(const std::string& name, const std::string& password, std::optional<ID_t> id = std::nullopt)
        : nickname_(name), passwordHash_(password), id_(id) {}

    void notify();

    void setID(ID_t newId) { id_ = newId; }
    void setName(const std::string name) { nickname_ = name; }
    void setPassword(const std::string password) { passwordHash_ = password; }

    std::optional<ID_t> getID() const { return id_; }
    
    std::string getName() const { return nickname_; }
    std::string getPassword() const { return passwordHash_; }

    bool operator==(const User&) const = default;
};