cmake_minimum_required(VERSION 3.20)
project(mqnotifysigl4)
add_definitions(-DHAVE_MQUEUE_H -DHAVE_SIGWAIT)
include_directories(../../lib)
set(CMAKE_CXX_STANDARD 17)
aux_source_directory(../../lib src)
add_executable(mqnotifysigl4 main.cpp ${src})
