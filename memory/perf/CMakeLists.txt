cmake_minimum_required(VERSION 3.16)

project(memory_perf)

add_executable(${PROJECT_NAME} perf.cpp)

target_link_libraries(${PROJECT_NAME} benchmark memory)

