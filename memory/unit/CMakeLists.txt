cmake_minimum_required(VERSION 3.16)

project(memory_unit)

add_executable(${PROJECT_NAME} unit.cpp)

target_link_libraries(${PROJECT_NAME} gtest memory)

