
project(CPSC_326_HW1)

cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-O0")
set(CMAKE_BUILD_TYPE Debug)

# build executable
add_executable(hw1 hw1.cpp)
