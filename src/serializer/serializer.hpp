#pragma once
#include <cstring>

#include "message/message.hpp"

class Serializer {
public:
    static std::vector<uint8_t> serialize(const Message& msg);

    static Message deserialize(const std::vector<uint8_t>& data);

private:
    template <typename T>
    static void push_value(std::vector<uint8_t>& data, const T& v);

    template <typename T>
    static void read_value(const std::vector<uint8_t>& data, size_t& offset, T& value);
};

template <typename T>
void Serializer::push_value(std::vector<uint8_t>& data, const T& v) {
    const uint8_t* p = reinterpret_cast<const uint8_t*>(&v);
    data.insert(data.end(), p, p + sizeof(T));
}

template <>
void Serializer::push_value<std::string>(std::vector<uint8_t>& data, const std::string& str);

template <typename T>
void Serializer::read_value(
    const std::vector<uint8_t>& data, size_t& offset, T& value
) {
    std::memcpy(&value, &data[offset], sizeof(T));
    offset += sizeof(T);
}

template <> 
void Serializer::read_value<std::string>(
    const std::vector<uint8_t>& data, size_t& offset, std::string& str
);