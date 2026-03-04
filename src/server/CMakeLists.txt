add_library(server_session_lib STATIC
    server_session/server_session.cpp 
    server_session/server_session.hpp
)
    
add_executable(server 
    main.cpp 
    server.cpp
    server.hpp
)

target_compile_options(server PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

if (ENABLE_CLANG_TIDY)
    set_target_properties(server PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-warnings-as-errors=-"
    )
endif()

target_link_libraries(server PRIVATE
    server_session_lib
    message_lib
    chat_lib
    db_lib
    tcp_socket_lib
)