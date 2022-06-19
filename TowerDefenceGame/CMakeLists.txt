cmake_minimum_required(VERSION 3.16)

project(TowerDefenceGame VERSION 0.1.0)

file(GLOB SOURCES
    "*.cpp"
)

file(GLOB SHADERS
    "shaders/*"
)

file(GLOB TEXTURES
    "textures/*"
)

include_directories(.)
include_directories(../)
include_directories(../common/include)
include_directories(../memory/include)
include_directories(../logger/include)
include_directories(../utils/include)
include_directories(../renderer/include)
include_directories(../window/include)


add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(
    ${PROJECT_NAME}
    common
    memory
    logger
    utils
    renderer
    window
    ${GL_LIBS}
)

add_custom_target(copy)

add_custom_command(
    TARGET copy PRE_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
)

add_custom_command(
    TARGET copy PRE_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/textures
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/textures
)

add_dependencies(${PROJECT_NAME} copy)