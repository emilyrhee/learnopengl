cmake_minimum_required(VERSION 3.10)
project(Test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/glad)
include_directories(${CMAKE_SOURCE_DIR}/include)

find_package(glfw3 REQUIRED)

add_executable(Test main.cpp glad.c)

target_link_libraries(Test PRIVATE glfw)