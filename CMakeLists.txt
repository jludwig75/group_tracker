cmake_minimum_required(VERSION 2.6.4)

project (group_tracker)

set(REPO_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-Wno-nonportable-include-path")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

add_subdirectory(googletest)
add_subdirectory(group_tracker)
