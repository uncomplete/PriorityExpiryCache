cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(proj VERSION 1.0)

# specify the C++ standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set gdb
SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_compile_options(-std=c++17)

add_executable(regular main.cpp)

add_executable(indexed main.cpp)
target_compile_definitions(indexed PUBLIC INDEX)
