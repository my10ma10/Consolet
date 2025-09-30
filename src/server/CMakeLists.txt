add_library(session_lib STATIC
    session/session.cpp 
    session/session.hpp
)
    
add_executable(server 
    main.cpp 
    server.cpp
    server.hpp
)

target_link_libraries(server PRIVATE
    session_lib
    message_lib
    chat_lib
)