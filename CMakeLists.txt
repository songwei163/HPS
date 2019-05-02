cmake_minimum_required(VERSION 3.14)
project(HPS)

set(CMAKE_CXX_STANDARD 17)

add_executable(byteorder byteorder.cpp)
add_executable(test_listen test_listen.cpp myutili.h)
