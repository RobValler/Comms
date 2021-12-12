cmake_minimum_required(VERSION 3.5.0)
project(comms_test)

find_package(GTest REQUIRED)
enable_testing()

set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}/../messages)

include_directories(${GTEST_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../comms_server/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../comms_client/src)
include_directories(${ProtobufIncludePath})

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${ProtobufIncludePath}/*.pb.cc
    )
file(GLOB_RECURSE HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${ProtobufIncludePath}/*.pb.h
    )

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} gtest_main gtest comms_client comms_server comms_messages)
