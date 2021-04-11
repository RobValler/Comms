cmake_minimum_required(VERSION 3.5.0)
project(comms_server)

FIND_PACKAGE(Protobuf REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../protocol)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../protocol/tcpip)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../DevFramework/Logger)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/tcpip/tcpip_server.cpp
    )

file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/iprotocol_server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../protocol/tcpip/tcpip_server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../DevFramework/Logger/Logger.h
    )

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
    )

target_link_libraries(${PROJECT_NAME}
    ${PROTOBUF_LIBRARY}
    )
