cmake_minimum_required(VERSION 3.21)
project(client2)
include_directories(../../lib)
set(CMAKE_CXX_STANDARD 17)
aux_source_directory(../../lib src)
add_executable(client2 main.cpp ${src})
