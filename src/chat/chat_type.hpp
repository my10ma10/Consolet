#pragma once
#include <string>
#include <unordered_map>

class ChatType {
public:
    enum class Type {
        PERSONAL,
        GROUP,
        UNKNOWN
    };

private:
    Type type_;

    const std::unordered_map<Type, std::string> map_ = {
        {Type::PERSONAL, "personal"},
        {Type::GROUP, "group"},
        {Type::UNKNOWN, nullptr}
    };

public:
    ChatType(Type type) : type_(type) {}

    std::string toString() {
        return map_.at(type_);
    }

    bool operator==(const ChatType& other) const { 
        return type_ == other.type_; 
    }
    bool operator!=(const ChatType& other) const { 
        return type_ != other.type_;
    }

};