cmake_minimum_required(VERSION 3.14)
project(Consolet)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug) 
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -Wextra -Wpedantic")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -Wall -Wextra -Wpedantic")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -O0 -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")
endif()

option(ENABLE_TESTS "Enable tests" ON)

add_subdirectory(external/spdlog)
add_subdirectory(external/json)


if (NOT TARGET SQLite3) 
    find_package(SQLite3 REQUIRED)
endif()
if (NOT TARGET spdlog) 
    find_package(spdlog REQUIRED)
endif()
if (NOT TARGET nlohmann_json) 
    find_package(nlohmann_json REQUIRED)
endif()

add_subdirectory(src)

if (ENABLE_TESTS) 
    add_subdirectory(external/googletest)
    enable_testing()
    add_subdirectory(tests)
endif()