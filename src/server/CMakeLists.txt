include_directories(
    ${CMAKE_SOURCE_DIR}/src/server
)

add_library(server_session_lib STATIC
    server_session/server_session.cpp 
    server_session/server_session.hpp
)
target_link_libraries(server_session_lib PUBLIC
    spdlog::spdlog
)

add_library(thread_pool_lib STATIC
    thread_pool/thread_pool.cpp
    thread_pool/thread_pool.hpp    
    thread_pool/thread_safe_queue.hpp
)
target_link_libraries(thread_pool_lib PUBLIC
    spdlog::spdlog
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
    db_lib
    tcp_socket_lib
)