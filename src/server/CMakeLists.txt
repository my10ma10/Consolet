add_library(server_session_lib STATIC
    server_session/server_session.cpp 
    server_session/server_session.hpp
)
    
add_executable(server 
    main.cpp 
    server.cpp
    server.hpp
)

target_link_libraries(server PRIVATE
    server_session_lib
    message_lib
    chat_lib
)