#/*****************************************************************
# * Copyright (C) 2017-2021 Robert Valler - All rights reserved.
# *
# * This file is part of the project: StarterApp
# *
# * This project can not be copied and/or distributed
# * without the express permission of the copyright holder
# *****************************************************************/

cmake_minimum_required(VERSION 3.5.0)
project(comms)
set (CMAKE_CXX_STANDARD 14)
set (Wfloat-conversion)
set (Wfno-stack-protector)
set (Winconsistent-missing-override)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_compile_options( -Wall -Wextra -pedantic)

FIND_PACKAGE(Protobuf REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../DevFramework/Logger)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/protocol)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/protocol/tcpip)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/protocol/posix_mq)

add_subdirectory(messages)
add_subdirectory(comms_server)
add_subdirectory(comms_client)
add_subdirectory(test)
