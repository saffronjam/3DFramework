cmake_minimum_required(VERSION 3.1.0)
project(ScriptCore)

set(CMAKE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_definitions(
            -DSE_PLATFORM_WINDOWS
    )
else()
    add_definitions(
            -DSE_PLATFORM_LINUX
    )
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_definitions(
            -DSE_DEBUG
    )
elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
    add_definitions(
            -DSE_RELEASE
    )
elseif(${CMAKE_BUILD_TYPE} MATCHES Dist)
    add_definitions(
            -DSE_DIST
    )
else()
    message( FATAL_ERROR "Bad configuration selected" )
endif()

file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/*.cs")
add_library(ScriptCore ${SRC_FILES})

target_include_directories(Editor PRIVATE "src")