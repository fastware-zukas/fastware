cmake_minimum_required(VERSION 3.16)

project(window VERSION 0.1.0)

file(GLOB SOURCES
    "src/*.cpp"
)

include_directories(./include)
include_directories(../)
include_directories(../3rd/stb)
include_directories(../common/include)
include_directories(../memory/include)
include_directories(../logger/include)


add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_link_libraries(${PROJECT_NAME} common memory logger ${GL_LIBS})