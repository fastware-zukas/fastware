#================================
# Project setup.
#================================
cmake_minimum_required(VERSION 3.30)
project(lua VERSION 5.4.7 LANGUAGES CXX C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#================================
# Option variables.
#================================
option(LUA_BUILD_COMPILER "Build Lua compiler" OFF)
option(LUA_BUILD_INTERPRETER "Build Lua interpreter" OFF)
option(LUA_INSTALL_TARGET "Create install target" OFF)

#================================
# Directory variables.
#================================
set(LUA_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(LUA_SOURCE_ROOT ${LUA_SOURCE_DIR}/src)
set(LUA_TEST_ROOT ${LUA_SOURCE_ROOT}/test)

#================================
# Add subdirectories.
#================================

set(LIB_SRC_LIST
    src/lapi.c
    src/lauxlib.c
    src/lbaselib.c
    src/lcode.c
    src/lcorolib.c
    src/lctype.c
    src/ldblib.c
    src/ldebug.c
    src/ldo.c
    src/ldump.c
    src/lfunc.c
    src/lgc.c
    src/linit.c
    src/liolib.c
    src/llex.c
    src/lmathlib.c
    src/lmem.c
    src/loadlib.c
    src/lobject.c
    src/lopcodes.c
    src/loslib.c
    src/lparser.c
    src/lstate.c
    src/lstring.c
    src/lstrlib.c
    src/ltable.c
    src/ltablib.c
    src/ltm.c
    src/lundump.c
    src/lutf8lib.c
    src/lvm.c
    src/lzio.c
)

set(LIB_INCLUDE_LIST
    include/lapi.h
    include/lauxlib.h
    include/lcode.h
    include/lctype.h
    include/ldebug.h
    include/ldo.h
    include/lfunc.h
    include/lgc.h
    include/ljumptab.h
    include/llex.h
    include/llimits.h
    include/lmem.h
    include/lobject.h
    include/lopcodes.h
    include/lopnames.h
    include/lparser.h
    include/lprefix.h
    include/lstate.h
    include/lstring.h
    include/ltable.h
    include/ltm.h
    include/lua.h
    include/luaconf.h
    include/lualib.h
    include/lundump.h
    include/lvm.h
    include/lzio.h
)

#================================
# Libraries.
#================================
set(LIBS)
if (UNIX AND NOT APPLE)
    include(CheckLibraryExists)
    check_library_exists(m sin "" HAVE_LIB_M)
    if (HAVE_LIB_M)
        list(APPEND LIBS m)
    endif()

endif()

#================================
# Lua library.
#================================
source_group("include" FILES ${LIB_INCLUDE_LIST})
source_group("source" FILES ${LIB_SRC_LIST})
add_library(liblua ${LIB_INCLUDE_LIST} ${LIB_SRC_LIST})
target_include_directories(liblua PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    )
if (UNIX AND NOT APPLE)
    target_link_libraries(liblua PUBLIC ${LIBS})
endif()
target_compile_features(liblua PRIVATE cxx_std_20)
set_target_properties(liblua PROPERTIES DEBUG_POSTFIX "d")
add_library(lua::lua ALIAS liblua)
set_target_properties(liblua PROPERTIES FOLDER "lua")

#================================
# Lua compiler.
#================================
if (LUA_BUILD_COMPILER)
    set(COMPILER_SRC src/luac.c)
    source_group("source" FILES ${COMPILER_SRC})
    add_executable(luac ${COMPILER_SRC})
    target_link_libraries(luac PRIVATE liblua)
    set_target_properties(luac PROPERTIES FOLDER "lua")
endif()

#================================
# Lua interpreter
#================================
if (LUA_BUILD_INTERPRETER)
    set(INTERP_SRC src/lua.c)
    source_group("source" FILES ${INTERP_SRC})
    add_executable(lua ${INTERP_SRC})
    target_link_libraries(lua PRIVATE liblua)
    set_target_properties(lua PROPERTIES FOLDER "lua")
endif()


set(LUA_INSTALL_INCLUDE
    ${LUA_SOURCE_ROOT}/lauxlib.h
    ${LUA_SOURCE_ROOT}/lua.h
    ${LUA_SOURCE_ROOT}/lua.hpp
    ${LUA_SOURCE_ROOT}/luaconf.h
    ${LUA_SOURCE_ROOT}/lualib.h
    PARENT_SCOPE)

#================================
# Install
#================================
if (LUA_INSTALL_TARGET)
    install(TARGETS liblua
        EXPORT liblua
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION lib
        INCLUDES DESTINATION include
        )

    include(CMakePackageConfigHelpers)
    include(GNUInstallDirs)
    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/luaConfigVersion.cmake"
        VERSION ${PACKAGE_VERSION}
        COMPATIBILITY ExactVersion
        )

    install(EXPORT liblua
        FILE luaTargets.cmake
        NAMESPACE lua::
        DESTINATION lib/cmake/lua
        )

    install(FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/luaConfig.cmake"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Findlua.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/luaConfigVersion.cmake"
        DESTINATION lib/cmake/lua
        )

    install(FILES
        ${LUA_INSTALL_INCLUDE}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lua"
        )

    if (MSVC)
        set(PDB_DIR_SUFFIX
            "$<$<CONFIG:Debug>:Debug>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>")
        set(LIB_SUFFIX "$<$<CONFIG:Debug>:d>")

        set(PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${PDB_DIR_SUFFIX}")
        set(PDB_NAME "liblua${LIB_SUFFIX}.pdb")
        install(FILES
            "${PDB_OUTPUT_DIRECTORY}/${PDB_NAME}"
            DESTINATION lib OPTIONAL)
    endif()

    if (LUA_BUILD_COMPILER)
        install(TARGETS luac
            DESTINATION bin
            )
    endif()

    if (LUA_BUILD_INTERPRETER)
        install(TARGETS lua
            DESTINATION bin
            )
    endif()
endif()
