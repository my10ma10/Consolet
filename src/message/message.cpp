#include "message.hpp"

Message::Message(ID_t chatID, ID_t senderID, const std::string& text, 
        int64_t timestamp, const std::optional<ID_t>& msgID
) : 
    chatID_(chatID), 
    senderID_(senderID), 
    text_(text), 
    timestamp_(timestamp), 
    msgID_(msgID)
{}

std::ostream& operator<<(std::ostream& stream, const Message& msg) {
    return stream << msg.getText();
}

Message::operator std::string() const {
    return this->getText();
}
