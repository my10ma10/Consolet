cmake_minimum_required(VERSION 3.17)
project(Consolet)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug) 
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -Wall -Wextra")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} --coverage")

find_package(SQLite3 REQUIRED)

option(ENABLE_TESTS "Enable tests" ON)

add_subdirectory(src)

if (ENABLE_TESTS) 
    add_subdirectory(external/googletest)
    enable_testing()
    add_subdirectory(tests)
endif()