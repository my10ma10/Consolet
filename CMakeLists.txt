cmake_minimum_required(VERSION 3.10)
project(Consolet)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLite3 REQUIRED)

add_subdirectory(src)
