
cmake_minimum_required(VERSION 3.16)

project(fastware VERSION 0.1.0)

set(BENCHMARK_ENABLE_TESTING OFF)

option(FASTWARE_SANITIZE_ADDRESS "Enable -fsanitize=address" OFF)
option(FASTWARE_SANITIZE_KERNEL_ADDRESS "Enable -fsanitize=kernel-address" OFF)
option(FASTWARE_SANITIZE_THREAD "Enable -fsanitize=thread" OFF)
option(FASTWARE_SANITIZE_LEAK "Enable -fsanitize=leak" OFF)
option(FASTWARE_SANITIZE_UNDEFINED "Enable -fsanitize=undefined" OFF)

if(FASTWARE_SANITIZE_ADDRESS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    list(APPEND GLOBAL_LIBS asan)
endif()

if(FASTWARE_SANITIZE_KERNEL_ADDRESS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=kernel-address")
endif()

if(FASTWARE_SANITIZE_THREAD)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    list(APPEND GLOBAL_LIBS tsan)
endif()

if(FASTWARE_SANITIZE_LEAK)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")
    list(APPEND GLOBAL_LIBS lsan)
endif()

if(FASTWARE_SANITIZE_UNDEFINED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    list(APPEND GLOBAL_LIBS ubsan)
endif()

add_compile_options(-msse4.2)

add_definitions(-DHAVE_STD_ATOMIC)

add_definitions(-DGLFW_INCLUDE_GLU)
add_definitions(-DGLFW_INCLUDE_GLU)
add_definitions(-DGLEW_STATIC)
add_definitions(-DSDL2_STATIC)

add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
add_definitions(-DGLM_FORCE_DEFAULT_ALIGNED_GENTYPES)


set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=x86-64 -fno-omit-frame-pointer -std=c++17 -fno-rtti -fno-exceptions")

include_directories(3rd/googletest/googletest/include)
include_directories(3rd/benchmark/include)
include_directories(3rd/glew/include)
include_directories(3rd/SDL2/include)
include_directories(3rd/glm)


set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

enable_testing()

add_subdirectory(3rd/googletest)
add_subdirectory(3rd/benchmark)
add_subdirectory(3rd/glew/build/cmake)
add_subdirectory(3rd/SDL2)
add_subdirectory(3rd/glm)
add_subdirectory(common)
add_subdirectory(memory)
add_subdirectory(logger)
add_subdirectory(utils)
add_subdirectory(renderer)
add_subdirectory(window)


SET(GL_LIBS ${OPENGL_LIBRARY} GLEW SDL2)

message("LIBRARIES: ${GL_LIBS}")

add_subdirectory(TowerDefenceGame)


