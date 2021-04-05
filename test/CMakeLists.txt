cmake_minimum_required(VERSION 3.5.0)
project(comms_test)

find_package(GTest REQUIRED)
enable_testing()

include_directories(${GTEST_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../comms_lib/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../messages)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../messages/*.pb.cc
    )
file(GLOB_RECURSE HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../messages/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../messages/*.pb.h
    )
file(GLOB_RECURSE PROTO
    ${CMAKE_CURRENT_SOURCE_DIR}/../messages/*.proto
    )

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${PROTO})
target_link_libraries(${PROJECT_NAME} gtest_main gtest comms_lib )
