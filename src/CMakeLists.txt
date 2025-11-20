include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/chat
    ${CMAKE_SOURCE_DIR}/src/client
    ${CMAKE_SOURCE_DIR}/src/command
    ${CMAKE_SOURCE_DIR}/src/message
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/usr
    &{CMAKE_SOURCE_DIR}/src/db
)

add_library(message_lib STATIC 
    message/message.cpp    
    message/message.hpp  
)

add_library(db_lib STATIC
    db/db.cpp
    db/db.hpp
)

target_compile_options(db_lib PRIVATE --coverage -O0 -g)
target_link_options(db_lib PRIVATE --coverage)
target_link_libraries(db_lib PUBLIC SQLite::SQLite3)


add_library(chat_lib STATIC 
    chat/chat.cpp
    chat/chat.hpp
)

target_compile_options(chat_lib PRIVATE --coverage -O0 -g)
target_link_options(chat_lib PRIVATE --coverage)

add_library(user_lib STATIC
    usr/user.cpp
    usr/user.hpp
)

target_compile_options(user_lib PRIVATE --coverage -O0 -g)
target_link_options(user_lib PRIVATE --coverage)

add_subdirectory(server)
add_subdirectory(client)