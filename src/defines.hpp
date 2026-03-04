#pragma once
#include <cstdint>
#include <string_view>

using ID_t = int64_t;
inline constexpr std::size_t BUF_SIZE = 4096;
inline constexpr std::size_t BACKLOG = 10;

namespace Defines {
inline constexpr std::string_view CLIENT_CACHE_DB_PATH = \
    "/assets/sql/create_client_cache_DB.sql";

inline constexpr std::string_view SERVER_DB_PATH = \
    "assets/sql/create_server_DB.sql";

}