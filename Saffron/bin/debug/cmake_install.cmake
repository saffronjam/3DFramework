# Install script for directory: C:/Users/ownem/source/repos/SaffronCmake/Saffron

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/SaffronProject")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/assimp/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/Box2D/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/entt/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/FastNoise/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/Glad/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/glfw/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/glm/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/imgui/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/mono/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/reactphysics3d/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/spdlog/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/stb/cmake_install.cmake")
  include("C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/deps/yaml-cpp/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/ownem/source/repos/SaffronCmake/Saffron/bin/debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
