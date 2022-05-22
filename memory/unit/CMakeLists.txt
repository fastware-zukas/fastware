cmake_minimum_required(VERSION 3.16)

project(memory_unit)

remove_definitions("-DNDEBUG")
add_definitions("-DFASTWARE_VERBOSE")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

include_directories(../include)

add_executable(${PROJECT_NAME} unit.cpp)

target_link_libraries(${PROJECT_NAME} gtest memory)

