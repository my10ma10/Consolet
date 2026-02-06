#include "serializer.hpp"

#include <optional>

std::vector<uint8_t> Serializer::serialize(const Message& msg) {
    std::vector<uint8_t> data;
    data.reserve(sizeof(Message));

    auto push_T = [&](auto&& x) {
        push_value(data, x);
    };

    if (msg.msgID_) push_T(*msg.msgID_); 
    else            push_T(std::nullopt);

    push_T(msg.chatID_);
    push_T(msg.senderID_);
    push_T(msg.text_);

    return data;
}

Message Serializer::deserialize(const std::vector<uint8_t>& data) {
    size_t offset = 0;
    Message msg;

    auto read_v = [&](auto&& v) {
        return read_value<decltype(v)>(data, offset, v);
    };

    read_v(msg.msgID_);
    read_v(msg.chatID_);
    read_v(msg.senderID_);
    read_v(msg.text_);
}

template <>
void Serializer::push_value<std::string>(std::vector<uint8_t>& data, const std::string& str) {
    push_value(data, str.size());

    data.insert(data.end(), str.begin(), str.end());
}

template <> 
void Serializer::read_value<std::string>(
    const std::vector<uint8_t>& data, size_t& offset, std::string& str
) {
    size_t len;
    read_value<size_t>(data, offset, len);

    str.assign(reinterpret_cast<const char*>(&data[offset]), len);
    offset += len;
}