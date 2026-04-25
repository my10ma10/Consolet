#pragma once
#include <iostream>

#include "db/db.hpp"

#include <chrono>
#include "nlohmann/json.hpp"
#include "nlohmann/adl_serializer.hpp"

namespace ch = std::chrono;

class Serializer;

class Message {
    friend class Serializer;
    friend class nlohmann::adl_serializer<Message>;

    ID_t chatID_;
    ID_t senderID_;
    std::string text_;
    int64_t timestamp_; // UTC milliseconds since Unix epoch
    std::optional<ID_t> msgID_;

public:
    Message() = default;
    
    Message(ID_t chatID, ID_t senderID, const std::string& text, 
        int64_t timestamp = ch::duration_cast<ch::milliseconds>(
            ch::system_clock::now().time_since_epoch()).count(),
        const std::optional<ID_t>& msgID = std::nullopt
    );

    void setID(ID_t id) { msgID_ = id; } 

    bool isSavedToDB() const { return msgID_.has_value(); }
    
    std::optional<ID_t> getID() const { return msgID_; }
    ID_t getSenderID() const { return senderID_; }
    ID_t getChatID() const { return chatID_; }
    std::string getText() const { return text_; }
    int64_t getTimeSinceEpoch() const { return timestamp_; }

    bool operator==(const Message& other) const = default;

    operator std::string() const;

    friend std::ostream& operator<<(std::ostream& stream, const Message& msg);

};