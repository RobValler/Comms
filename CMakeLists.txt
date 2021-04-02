cmake_minimum_required(VERSION 3.5.0)
project(comms)
set (CMAKE_CXX_STANDARD 14)
set (Wall)
set (W1)
set (Wfloat-conversion)
set (Wfno-stack-protector)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread ")


add_subdirectory(comms_lib)
add_subdirectory(test)
