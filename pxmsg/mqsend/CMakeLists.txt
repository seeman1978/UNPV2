cmake_minimum_required(VERSION 3.20)
project(mqsend)
add_definitions(-DHAVE_MQUEUE_H)
include_directories(../../lib)
set(CMAKE_CXX_STANDARD 17)
aux_source_directory(../../lib src)
add_executable(mqsend main.cpp ${src})
