cmake_minimum_required(VERSION 3.2)
project(congestion_client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES client.cpp)
add_executable(congestion_client ${SOURCE_FILES} congestion.h congestion.cpp)
target_link_libraries(congestion_client pthread)