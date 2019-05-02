cmake_minimum_required(VERSION 3.14)
project(HPS)

set(CMAKE_CXX_STANDARD 17)

add_executable(byteorder byteorder.cpp test_accept.cpp oobsend.cpp oobrecv.cpp reuse_address.cpp)
add_executable(test_listen test_listen.cpp myutili.h)
add_executable(test_accept test_listen.cpp myutili.h)
add_executable(oobsend oobsend.cpp myutili.h)
add_executable(oobrecv oobrecv.cpp myutili.h)
add_executable(reuse_address reuse_address.cpp myutili.h)