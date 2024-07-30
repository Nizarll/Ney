cmake_minimum_required(VERSION 3.3...3.19)

project(ney)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS "-g3 -Wall -Wpedantic")

set(SOURCES src/main.cpp src/parser.cpp)
add_executable(app ${SOURCES})
