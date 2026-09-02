cmake_minimum_required(VERSION 3.14)

# Allow this directory to be configured either through the repository root or
# directly with `cmake -S Project-2DGame -B <build-directory>`.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(
        Custom2DPlatformer
        VERSION 1.0
        DESCRIPTION "CS202 OOP Platformer Game"
        LANGUAGES CXX
    )
endif()

set(GAME_TARGET Custom2DPlatformer)
set(GAME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

# Build the pinned SFML dependency statically. Network support is not used by
# the game, so excluding it shortens clean builds.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build dependencies as static libraries" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "Build the SFML network module" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "Build SFML examples" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "Build SFML documentation" FORCE)

include(FetchContent)

if(NOT TARGET sfml-graphics)
    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
        GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SFML)
endif()

# CONFIGURE_DEPENDS makes CMake regenerate when new implementation or header
# files are added below the refactored project directory.
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    "${GAME_ROOT}/src/*.cpp"
)
file(GLOB_RECURSE GAME_HEADERS CONFIGURE_DEPENDS
    "${GAME_ROOT}/include/*.h"
    "${GAME_ROOT}/include/*.hpp"
)

if(NOT GAME_SOURCES)
    message(FATAL_ERROR "No C++ sources were found under ${GAME_ROOT}/src")
endif()

add_executable(${GAME_TARGET} ${GAME_SOURCES} ${GAME_HEADERS})
target_compile_features(${GAME_TARGET} PRIVATE cxx_std_17)

# Some existing files include local names such as "Hero.h" and
# "GiantForm.h". Expose every header directory until those includes are
# normalized to paths relative to the include root.
set(GAME_INCLUDE_DIRS "${GAME_ROOT}/include")
foreach(header IN LISTS GAME_HEADERS)
    get_filename_component(header_directory "${header}" DIRECTORY)
    list(APPEND GAME_INCLUDE_DIRS "${header_directory}")
endforeach()
list(REMOVE_DUPLICATES GAME_INCLUDE_DIRS)

target_include_directories(${GAME_TARGET} PRIVATE ${GAME_INCLUDE_DIRS})

target_link_libraries(${GAME_TARGET} PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

if(NOT IS_DIRECTORY "${GAME_ROOT}/assets")
    message(FATAL_ERROR "Game assets were not found under ${GAME_ROOT}/assets")
endif()

# Runtime asset paths in the code are relative to the executable.
add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${GAME_ROOT}/assets"
            "$<TARGET_FILE_DIR:${GAME_TARGET}>/assets"
    COMMENT "Copying game assets beside ${GAME_TARGET}"
    VERBATIM
)

# Static SFML audio still uses OpenAL as a runtime DLL on Windows.
if(WIN32 AND DEFINED SFML_SOURCE_DIR)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(SFML_OPENAL_ARCH "x64")
    else()
        set(SFML_OPENAL_ARCH "x86")
    endif()

    set(SFML_OPENAL_DLL
        "${SFML_SOURCE_DIR}/extlibs/bin/${SFML_OPENAL_ARCH}/openal32.dll"
    )

    if(EXISTS "${SFML_OPENAL_DLL}")
        add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${SFML_OPENAL_DLL}"
                    "$<TARGET_FILE_DIR:${GAME_TARGET}>/openal32.dll"
            COMMENT "Copying the OpenAL runtime beside ${GAME_TARGET}"
            VERBATIM
        )
    else()
        message(WARNING "OpenAL runtime not found at ${SFML_OPENAL_DLL}")
    endif()
endif()
