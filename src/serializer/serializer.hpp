#pragma once
#include <cstring>
#include <string_view>

#include <spdlog/spdlog.h>

#include "message/message.hpp"

class Serializer {
public:
    static std::string serialize(const Message& msg);

    static Message deserialize(const std::string_view& data);
};
