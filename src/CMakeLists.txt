include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/client
)

add_library(json_lib STATIC 
    json/json.cpp
)  
target_link_libraries(json_lib PUBLIC 
    nlohmann_json
)


add_library(message_lib STATIC 
    message/message.cpp    
    message/message.hpp  
)
target_link_libraries(message_lib PUBLIC 
    json_lib
)

add_library(user_lib STATIC
    user/user.cpp
    user/user.hpp
)

add_library(chat_lib STATIC 
    chat/chat.cpp
    chat/chat.hpp
)


add_library(db_lib STATIC
    db/db.cpp
    db/db.hpp
    db/client_cache_db.hpp
    db/client_cache_db.cpp
    db/server_db.hpp
    db/server_db.cpp
)
target_link_libraries(db_lib PUBLIC 
    SQLite::SQLite3 
    spdlog::spdlog
    chat_lib
    message_lib
)


add_library(command_lib STATIC
    command/adapters.cpp
    command/adapters.hpp
    command/command_factory.cpp
    command/command_factory.hpp
    command/command_interface.cpp
    command/command_interface.hpp
    command/command.cpp
    command/command.hpp
)
target_link_libraries(command_lib PUBLIC 
    spdlog::spdlog
    message_lib
)



add_library(tcp_socket_lib STATIC
    tcp_socket/tcp_socket.cpp
    tcp_socket/tcp_socket.hpp
)
target_link_libraries(tcp_socket_lib PUBLIC 
    spdlog::spdlog
)


add_library(serializer_lib STATIC
    serializer/serializer.cpp
    serializer/serializer.hpp
)
target_link_libraries(serializer_lib PUBLIC 
    message_lib
)


add_subdirectory(server)
add_subdirectory(client)