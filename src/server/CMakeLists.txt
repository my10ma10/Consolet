include_directories(
    ${CMAKE_SOURCE_DIR}/src/server
)

add_library(thread_pool_lib STATIC
    thread_pool/thread_pool.cpp
    thread_pool/thread_pool.hpp    
    thread_pool/thread_safe_queue.hpp
)
target_link_libraries(thread_pool_lib PUBLIC
    spdlog::spdlog
)

add_library(client_session_lib STATIC
    client_session/client_session.cpp
    client_session/client_session.hpp
)
target_link_libraries(client_session_lib PUBLIC
    spdlog::spdlog
    message_lib
)

add_executable(server 
    main.cpp 
    server.cpp
    server.hpp
)
target_link_libraries(server PRIVATE
    client_session_lib
    chat_lib
    db_lib
    tcp_socket_lib
)