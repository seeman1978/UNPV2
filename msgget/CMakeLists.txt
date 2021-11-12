cmake_minimum_required(VERSION 3.20)
project(msgget)
include_directories(../lib)
set(CMAKE_CXX_STANDARD 17)

add_executable(msgget main.cpp)
