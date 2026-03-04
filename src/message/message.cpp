#include "message.hpp"

std::ostream& operator<<(std::ostream& stream, const Message& msg) {
    return stream << msg.getText();
}

Message::operator std::string() const {
    return this->getText();
}
