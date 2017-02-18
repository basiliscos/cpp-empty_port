cmake_minimum_required(VERSION 2.8)
project (empty-port)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include (CTest)
enable_testing()

include_directories(${empty-port_SOURCE_DIR})

add_executable(t-10-tcp t/10-tcp.cpp)
add_test("t-10-tcp" t-10-tcp)

