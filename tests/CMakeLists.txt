add_executable(tests 
    main_test.cpp
    db_test.cpp
    chat_test.cpp
)

target_include_directories(tests PUBLIC
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(tests
    PRIVATE
    db_lib
    chat_lib
    gtest_main
    gmock_main
)

target_compile_definitions(tests 
    PRIVATE 
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

include(GoogleTest)