cmake_minimum_required(VERSION 3.5.0)
project(comms_lib)

FIND_PACKAGE(Protobuf REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/protocol)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/protocol/tcpip)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol/tcpip/*.cpp
    )

file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol/tcpip/*.h
    )

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
    ${HEADERS}
    )

target_link_libraries(${PROJECT_NAME}
    ${PROTOBUF_LIBRARY}
    )
