find_package(Curses REQUIRED)


add_library(ui_lib STATIC
    ui/ui.hpp 
    ui/ui.cpp
)
target_link_libraries(ui_lib PUBLIC 
    tcp_socket_lib
    ${CURSES_LIBRARIES}
)
target_include_directories(ui_lib PUBLIC
    ${CURSES_INCLUDE_DIR}
)


add_library(client_connection_lib STATIC
    client_connection/client_connection.cpp
    client_connection/client_connection.hpp
)
target_link_libraries(client_connection_lib PUBLIC 
    spdlog::spdlog
    message_lib
)


add_executable(client 
    main.cpp 
    client.cpp
    client.hpp
)
target_compile_definitions(client PRIVATE 
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)
target_link_libraries(client PRIVATE
    spdlog::spdlog
    client_connection_lib
    ui_lib
    message_lib
    chat_lib
    serializer_lib
    db_lib
    command_lib
)