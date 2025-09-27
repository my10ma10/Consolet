#include "message.hpp"

Message::Message(const std::string &sender_name, const std::string &text)
    : _senderName(sender_name), _text(text)
    {}

std::string Message::getSenderName() const
{
    return _senderName;
}

std::string Message::getText() const
{
    return _text;
}
