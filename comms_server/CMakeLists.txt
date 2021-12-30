#/*****************************************************************
# * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
# *
# * This file is part of the project: comms_server
# *
# * This project can not be copied and/or distributed
# * without the express permission of the copyright holder
# *****************************************************************/
cmake_minimum_required(VERSION 3.15.0)
project(comms_server)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/tcpip/tcpip_server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/tcpip/tcpip_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../serialiser/protobuf/proto_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../serialiser/basic/basic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/posix_mq/posix_mq_server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/posix_mq/posix_mq_helper.cpp
    )

file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/iprotocol_server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/tcpip/tcpip_server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/tcpip/tcpip_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../serialiser/iserialiser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../serialiser/protobuf/proto_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../serialiser/basic/basic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/posix_mq/posix_mq_server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/posix_mq/posix_mq_helper.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../DevFramework/Logger/Logger.h
    )

# compile
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARY} rt)

