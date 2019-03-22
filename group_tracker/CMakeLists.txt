set(GROUP_TRACKER_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${REPO_ROOT_DIR}/group_tracker/)
include_directories(${REPO_ROOT_DIR}/gmock-global/include/)
include_directories(${REPO_ROOT_DIR}/platform/include)
include_directories(${REPO_ROOT_DIR}/platform/libraries/include)

set(UNIT_TEST_NAME location_unittest)
cxx_executable(${UNIT_TEST_NAME} test gmock_main location.cpp)
add_custom_command(
     TARGET ${UNIT_TEST_NAME}
     COMMENT "${UNIT_TEST_NAME} uint tests"
     POST_BUILD 
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
     COMMAND ${UNIT_TEST_NAME}
)


add_library(group_tracker
                group_locator.cpp
                location.cpp
                location_list.cpp
                location_listener.cpp
                location_sender.cpp
                location_sender_ops.cpp
                lora_location_sender.cpp
                worker.cpp)