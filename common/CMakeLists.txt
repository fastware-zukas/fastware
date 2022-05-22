cmake_minimum_required(VERSION 3.16)

project(common VERSION 0.1.0)

file(GLOB SOURCES
    "src/*.cpp"
)

include_directories(./include)

add_library(${PROJECT_NAME} STATIC ${SOURCES})