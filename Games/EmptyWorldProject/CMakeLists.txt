cmake_minimum_required(VERSION 3.1.0)
project(EmptyWorldProject)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

file(GLOB_RECURSE SRC_FILES "src/*.cs")
add_library(EmptyWorldProject SHARED ${SRC_FILES})

add_dependencies(EmptyWorldProject ScriptCore)

target_include_directories(EmptyWorldProject PRIVATE "src")