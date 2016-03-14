cmake_minimum_required(VERSION 3.2)
project(congestion)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES server.cpp)
add_executable(congestion ${SOURCE_FILES} congestion.h congestion.cpp)
target_link_libraries(congestion pthread)