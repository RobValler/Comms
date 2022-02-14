get_filename_component(SELF_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(${SELF_DIR}/Comms-targets.cmake)
get_filename_component(COMMS_INCLUDE_DIRS "${SELF_DIR}/include" ABSOLUTE)

