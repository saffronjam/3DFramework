cmake_minimum_required(VERSION 3.1.0)
project(Saffron VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")

file(COPY "res" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

set(THREADS_PREFER_PTHREAD_FLAG ON)

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
    file(COPY "deps/assimp/bin/Debug/assimp-vc141-mtd.dll" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
    add_definitions(
            -DSE_RELEASE
    )
    file(COPY "deps/assimp/bin/Release/assimp-vc141-mtd.dll" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
elseif(${CMAKE_BUILD_TYPE} MATCHES Dist)
    add_definitions(
            -DSE_DIST
    )
    file(COPY "deps/assimp/bin/Release/assimp-vc141-mtd.dll" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
else()
    message( FATAL_ERROR "Bad configuration selected" )
endif()

add_subdirectory("deps/assimp")
add_subdirectory("deps/Box2D")
add_subdirectory("deps/entt")
add_subdirectory("deps/FastNoise")
add_subdirectory("deps/Glad")
add_subdirectory("deps/glfw")
add_subdirectory("deps/glm")
add_subdirectory("deps/imgui")
add_subdirectory("deps/mono")
add_subdirectory("deps/reactphysics3d")
add_subdirectory("deps/spdlog")
add_subdirectory("deps/stb")
add_subdirectory("deps/yaml-cpp")

file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/*.cpp")
add_executable(Saffron ${SRC_FILES} src/Saffron/EntryPoint.h)

target_precompile_headers(Saffron PUBLIC "${CMAKE_SOURCE_DIR}/SaffronPCH.h")

set(VENDOR_INCLUDES
        "${Assimp_INCLUDES}"
        "${Box2D_INCLUDES}"
        "${Entt_INCLUDES}"
        "${FastNoise_INCLUDES}"
        "${Glad_INCLUDES}"
        "${GLFW_INCLUDES}"
        "${GLM_INCLUDES}"
        "${ImGui_INCLUDES}"
        "${Mono_INCLUDES}"
        "${ReactPhysics3D_INCLUDES}"
        "${Spdlog_INCLUDES}"
        "${Stb_INCLUDES}"
        "${YAMLCPP_INCLUDES}"
        )

target_include_directories(Saffron PRIVATE
        "src"
        ${VENDOR_INCLUDES}
        )

target_link_libraries(Saffron
        Box2D
        FastNoise
        Glad
        GLFW
        ImGui
        ReactPhysics3D
        YAMLCPP
        "${PROJECT_SOURCE_DIR}/deps/mono/lib/Debug/mono-2.0-sgen.lib"
        "${PROJECT_SOURCE_DIR}/deps/assimp/lib/Debug/assimp-vc141-mtd.lib"
        )

file(GLOB INCLUDES "src")
