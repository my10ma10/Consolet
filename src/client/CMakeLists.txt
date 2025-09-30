add_library(ui_lib STATIC
    ui/ui.hpp 
    ui/ui.cpp
)
    
add_executable(client 
    main.cpp 
    client.cpp
    client.hpp
)

target_link_libraries(client PRIVATE
    ui_lib
    message_lib
    chat_lib
)