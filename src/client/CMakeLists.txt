add_executable(client 
    main.cpp 
    client.cpp
    client.hpp
)

target_compile_definitions(client 
    PRIVATE 
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

add_library(ui_lib STATIC
    ui/ui.hpp 
    ui/ui.cpp
)

add_library(client_session_lib STATIC
    client_session/client_session.cpp 
    client_session/client_session.hpp
)

target_compile_definitions(client_session_lib 
    PRIVATE 
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(client_session_lib
    PUBLIC command_lib
)

add_library(client_connection_lib STATIC
    client_connection/client_connection.cpp
    client_connection/client_connection.hpp
)

target_link_libraries(client_connection_lib PUBLIC spdlog::spdlog)
target_link_libraries(ui_lib PUBLIC tcp_socket_lib)


find_package(Curses REQUIRED)

if (ENABLE_CLANG_TIDY)
    set_target_properties(client PROPERTIES
        CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-warnings-as-errors=-"
    )
endif()

target_link_libraries(ui_lib PUBLIC
    ${CURSES_LIBRARIES}
)

target_include_directories(ui_lib PUBLIC
    ${CURSES_INCLUDE_DIR}
)

target_link_libraries(client PRIVATE
    spdlog::spdlog
    client_session_lib
    client_connection_lib
    ui_lib
    message_lib
    chat_lib
    serializer_lib
    db_lib
)

target_compile_options(client PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
