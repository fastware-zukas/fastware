cmake_minimum_required(VERSION 3.16)

project(logger)

file(GLOB SOURCES
    "src/*.cpp"
)
include_directories(.)
include_directories(include)
include_directories(../)
include_directories(../common/include)
include_directories(../memory/include)


add_library(${PROJECT_NAME} STATIC ${SOURCES} ${PRIVATE_SOURCES})
target_link_libraries(${PROJECT_NAME} common memory)
