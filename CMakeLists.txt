cmake_minimum_required(VERSION 3.10)
project(rint)

include_directories(${CMAKE_SOURCE_DIR})

add_compile_options(-std=c++17)

add_subdirectory(tests)
add_executable(rinitmain main.cpp)
