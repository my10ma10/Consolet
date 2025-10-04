add_library(ui_lib STATIC
    ui/ui.hpp 
    ui/ui.cpp
)

add_library(client_session_lib STATIC
    client_session/client_session.cpp 
    client_session/client_session.hpp
)
    
add_executable(client 
    main.cpp 
    client.cpp
    client.hpp
)

target_link_libraries(client PRIVATE
    client_session_lib
    ui_lib
    message_lib
    chat_lib
)