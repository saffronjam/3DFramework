cmake_minimum_required(VERSION 3.1.0)
project(SaffronEngine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Copy dlls
else ()
    # FindPackage
endif ()

file(COPY "res/Shaders" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

find_package(Threads REQUIRED)

add_definitions(
        # Static or dynamic?
        -DGLEW_STATIC
)


file(GLOB_RECURSE ALL_SOURCES "${CMAKE_SOURCE_DIR}/*.cpp")

set(ClientSRC "${ALL_SOURCES}")

add_executable(Client "${ClientSRC}")

target_include_directories(Client PRIVATE
        src
        src/Apps
        src/Bindables
        src/Drawables
        src/Etc
        src/GL
        src/Graphics
        src/Gui
        src/Input
        src/Interfaces
        src/Primitives
        src/Project
        src/Res
        src/Screens
        src/Shaders
        src/System
        src/ThrowMacros
        )

set_target_properties(Client PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
        )

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_include_directories(Client PRIVATE
            deps/GLEW/include
            deps/GLFW/include
            deps/glm/include
            )
    target_link_directories(Client PRIVATE
            deps/GLEW/lib
            deps/GLFW/lib
            )
    target_link_libraries(Client PRIVATE
            gdi32
            glfw3
            glew32s
            opengl32
            )
else ()
    target_include_directories(Client PRIVATE
            )
    target_link_directories(Client PRIVATE
            )
    target_link_libraries(Client PRIVATE
            )
endif ()