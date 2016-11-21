cmake_minimum_required(VERSION 2.8)

set(SRC_LIST ../src/txt_protocol.cpp
             ../src/engine.cpp
             ../src/notifier.cpp
             ./tests.cpp
             )

add_executable(tests ${SRC_LIST})
