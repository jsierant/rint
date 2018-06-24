cmake_minimum_required(VERSION 3.7)
project(rint)

include_directories(${CMAKE_SOURCE_DIR})

add_compile_options(-std=c++17 -Wall -Wextra -pedantic)

add_subdirectory(tests)
