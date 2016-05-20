cmake_minimum_required(VERSION 3.3)
project(HideInBmp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -std=c++11")

set(SOURCE_FILES HideInBmp.cpp)
add_executable(HideInBmp ${SOURCE_FILES})