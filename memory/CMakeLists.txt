cmake_minimum_required(VERSION 3.16)

project(memory VERSION 0.1.0)

file(GLOB SOURCES
    "src/*.cpp"
)

include_directories(../)
include_directories(include)


add_library(${PROJECT_NAME} STATIC ${SOURCES})

add_subdirectory(unit)
add_subdirectory(perf)

add_test(NAME unit COMMAND memory_unit)
add_test(NAME perf COMMAND memory_perf)