cmake_minimum_required(VERSION 3.8)
project(process VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

# Make main and test executable
if(UNIX AND NOT APPLE)
    add_definitions(-DLINUX)
    add_executable(main main.cpp process.cpp)
    add_executable(test test.cpp process.cpp)
else()
    add_definitions(-DOSX)
    add_executable(main main.cpp process.cpp pipe2_macos.cpp)
    add_executable(test test.cpp process.cpp pipe2_macos.cpp)
endif()