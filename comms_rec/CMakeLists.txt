cmake_minimum_required(VERSION 3.5.0)
project ( comms_rec )
set (CMAKE_CXX_STANDARD 14)
set (Wall)
set (W1)
set (Wfloat-conversion)

FIND_PACKAGE(Protobuf REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../common)
include_directories(${PROTOBUF_INCLUDE_DIR})


file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/*.pb.cc
)

file(GLOB HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/*.pb.h
)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    )

target_link_libraries(${PROJECT_NAME}
    ${PROTOBUF_LIBRARY}
    )
