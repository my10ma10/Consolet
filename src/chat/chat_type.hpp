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

    static const std::unordered_map<Type, std::string>& getTypeToStringMap() {
        static const std::unordered_map<Type, std::string> map = {
            {Type::PERSONAL, "personal"},
            {Type::GROUP, "group"},
            {Type::UNKNOWN, "unknown"}
        };
        return map;
    }
    
    static const std::unordered_map<std::string, Type>& getStringToTypeMap() {
        static const std::unordered_map<std::string, Type> map = {
            {"personal", Type::PERSONAL},
            {"group", Type::GROUP},
            {"unknown", Type::UNKNOWN}
        };
        return map;
    }

public:
    ChatType() : type_(Type::UNKNOWN) {}
    ChatType(Type type) : type_(type) {}
    ChatType(const std::string& str_type) : type_(fromString(str_type)) {}
    
    
    static std::string toString(Type type) {
        return getTypeToStringMap().at(type);
    }

    static Type fromString(const std::string& str) {
        return getStringToTypeMap().at(str);
    }

    std::string toString() const {
        return toString(type_);
    }

    Type getType() const { return type_; }

    bool operator==(const ChatType& other) const { 
        return type_ == other.type_; 
    }
    bool operator!=(const ChatType& other) const { 
        return type_ != other.type_;
    }
    
};