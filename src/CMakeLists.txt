include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/chat
    ${CMAKE_SOURCE_DIR}/src/client
    ${CMAKE_SOURCE_DIR}/src/command
    ${CMAKE_SOURCE_DIR}/src/message
    ${CMAKE_SOURCE_DIR}/src/server
    ${CMAKE_SOURCE_DIR}/src/usr
)

add_library(message_lib STATIC 
    message/message.cpp    
    message/message.hpp  
)

add_library(chat_lib STATIC 
    chat/chat.cpp
    chat/chat.hpp
)


add_subdirectory(server)
add_subdirectory(client)