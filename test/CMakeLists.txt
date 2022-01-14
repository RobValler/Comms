#/*****************************************************************
# * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
# *
# * This file is part of the project: Comms
# *
# * This project can not be copied and/or distributed
# * without the express permission of the copyright holder
# *****************************************************************/
cmake_minimum_required(VERSION 3.15.0)
project(comms_test)

find_package(GTest REQUIRED)
enable_testing()

set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}/../messages)

include_directories(${GTEST_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../comms_server/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../comms_client/src)
include_directories(${ProtobufIncludePath})

#server
file(GLOB_RECURSE SERVER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_tcpip_server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server_main.cpp
    ${ProtobufIncludePath}/*.pb.cc
    )
file(GLOB_RECURSE SERVER_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common.h
    ${ProtobufIncludePath}/*.pb.h
    )

add_executable("${PROJECT_NAME}_server" ${SERVER_SOURCES} ${SERVER_HEADERS})
target_link_libraries("${PROJECT_NAME}_server" gtest_main gtest comms_server comms_messages)


#client
file(GLOB_RECURSE CLIENT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_tcpip_client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client_main.cpp
    ${ProtobufIncludePath}/*.pb.cc
    )
file(GLOB_RECURSE CLIENT_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common.h
    ${ProtobufIncludePath}/*.pb.h
    )

add_executable("${PROJECT_NAME}_client" ${CLIENT_SOURCES} ${CLIENT_HEADERS})
target_link_libraries("${PROJECT_NAME}_client" gtest_main gtest comms_client comms_messages)
