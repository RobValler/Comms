cmake_minimum_required(VERSION 3.5.0)
project(comms_messages)

FIND_PACKAGE(Protobuf REQUIRED)

set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL /messages)
include_directories(${ProtobufIncludePath})

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/src/*.proto")
PROTOBUF_GENERATE_CPP(ProtoSources ProtoHeaders ${ProtoFiles})
add_library(${PROJECT_NAME} STATIC ${ProtoSources} ${ProtoHeaders} ${ProtoFiles})
target_link_libraries(${PROJECT_NAME} ${PROTOBUF_LIBRARY})
