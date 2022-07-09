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

#set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}/../messages)
set(ProtobufIncludePath ${CMAKE_BINARY_DIR}/messages/src)


include_directories(${GTEST_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../comms_server/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../comms_client/src)
include_directories(${ProtobufIncludePath})
message("---->" ${ProtobufIncludePath})

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_tcpip.cpp
    ${ProtobufIncludePath}/src/*.pb.cc
    )
file(GLOB_RECURSE HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common.h
    ${ProtobufIncludePath}/src/*.pb.h
    )

add_executable("${PROJECT_NAME}" ${SOURCES} ${HEADERS})
target_link_libraries("${PROJECT_NAME}" gtest
                                        CONAN_PKG::protobuf
                                        comms_messages
                                        comms_client
                                        comms_server)


#server
#file(GLOB_RECURSE SERVER_SOURCES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_tcpip_server.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/server_main.cpp
#    ${ProtobufIncludePath}/*.pb.cc
#    )
#file(GLOB_RECURSE SERVER_HEADERS
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/common.h
#    ${ProtobufIncludePath}/*.pb.h
#    )

#add_executable("${PROJECT_NAME}_server" ${SERVER_SOURCES} ${SERVER_HEADERS})
#target_link_libraries("${PROJECT_NAME}_server" ${CONAN_LIBS} gtest comms_server comms_messages)


##client
#file(GLOB_RECURSE CLIENT_SOURCES
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/test_tcpip_client.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/client_main.cpp
#    ${ProtobufIncludePath}/*.pb.cc
#    )
#file(GLOB_RECURSE CLIENT_HEADERS
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/common.h
#    ${ProtobufIncludePath}/*.pb.h
#    )

#add_executable("${PROJECT_NAME}_client" ${CLIENT_SOURCES} ${CLIENT_HEADERS})
#target_link_libraries("${PROJECT_NAME}_client" ${CONAN_LIBS} gtest comms_client comms_messages)
