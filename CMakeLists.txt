cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++1y -pthread")

find_package( Boost 1.55 REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

set(SRC_LIST ./src/main.cpp
             ./src/txt_protocol.cpp
             ./src/engine.cpp
             ./src/notifier.cpp
             )

add_executable(app ${SRC_LIST})
add_subdirectory(test)
