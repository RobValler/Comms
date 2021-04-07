cmake_minimum_required(VERSION 3.5.0)
project(comms)
set (CMAKE_CXX_STANDARD 14)
set (Wfloat-conversion)
set (Wfno-stack-protector)
set (Winconsistent-missing-override)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread ")

add_compile_options( -Wall -Wextra -pedantic)

add_subdirectory(messages)
add_subdirectory(comms_lib)
add_subdirectory(test)
