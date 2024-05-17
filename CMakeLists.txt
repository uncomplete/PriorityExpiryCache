cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(PriorityExpiryCache VERSION 1.0)

find_package(Boost 1.71.0)
include_directories(${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 23)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -stdlib=libc++")

add_executable(tests tests/test_cache.cpp)

#add_executable(main main.cpp)
