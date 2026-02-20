#pragma once
#include <iostream>

#include "db/db.hpp"

class Serializer;

class Message {
    friend class Serializer;

    ID_t chatID_;
    ID_t senderID_;
    std::string text_;
    std::optional<ID_t> msgID_;

public:
    Message() = default;
    
    Message(ID_t chatID, ID_t senderID, const std::string& text, 
        const std::optional<ID_t>& msgID = std::nullopt
    ) 
        : chatID_(chatID), senderID_(senderID), text_(text), msgID_(msgID)
    {}

    void setID(ID_t id) { msgID_ = id; } 

    bool isSavedToDB() const { return msgID_.has_value(); }
    
    std::optional<ID_t> getID() const { return msgID_; }
    ID_t getSenderID() const { return senderID_; }
    ID_t getChatID() const { return chatID_; }
    std::string getText() const { return text_; }

    bool operator==(const Message& other) const = default;

};