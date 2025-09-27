#pragma once
#include <iostream>

class Message {
    std::string _senderName;
    std::string _text;

public:
    Message(const std::string& sender_name, const std::string& text);
    
    std::string getSenderName() const;
    std::string getText() const;

};