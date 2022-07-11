#/*****************************************************************
# * Copyright (C) 2017-2022 Robert Valler - All rights reserved.
# *
# * This file is part of the project: Communication platform
# *
# * This project can not be copied and/or distributed
# * without the express permission of the copyright holder
# *****************************************************************/
cmake_minimum_required(VERSION 3.15.0)
project(Comms)
set (CMAKE_CXX_STANDARD 17)
set (Wfloat-conversion)
set (Wfno-stack-protector)
set (Winconsistent-missing-override)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    add_compile_options( -Wall -Wextra -Werror)
    message("The build config is DEBUG")
else()
    add_compile_options( -Wall -Wextra)
    message("The build config is RELEASE")
endif()

include_directories(${CONAN_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/protocol)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/protocol/tcpip)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/serialiser)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/serialiser/protobuf)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/serialiser/basic)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/protocol/posix_mq)

add_subdirectory(messages)
add_subdirectory(comms_server)
add_subdirectory(comms_client)
add_subdirectory(test)

install(TARGETS comms_client DESTINATION lib/comms)
install(TARGETS comms_server DESTINATION lib/comms)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/comms_client/src/comm_client.h DESTINATION include/comms)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/comms_server/src/comm_server.h DESTINATION include/comms)
#install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/CommsConfig.cmake DESTINATION .)
#install(FILES ${CMAKE_BINARY_DIR}/CommsTargets.cmake DESTINATION .)
#install(FILES ${CMAKE_BINARY_DIR}/CommsConfigVersion.cmake DESTINATION .)
