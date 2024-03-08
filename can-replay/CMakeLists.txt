cmake_minimum_required(VERSION 3.10)
set(CSV_CXX_STANDARD 20)
project(CanReplay)

include_directories(include)

file(GLOB SRC_CPP src/*.cpp)
file(GLOB SRC_C src/*.c)

add_executable(canreplay ${SRC_CPP} ${SRC_C})

