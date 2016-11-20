cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++1y -pthread -g")

set(SRC_LIST ./src/main.cpp
             ./src/txt_protocol.cpp
             ./src/engine.cpp
             )

add_executable(app ${SRC_LIST})

add_subdirectory(test)
