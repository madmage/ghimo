cmake_minimum_required(VERSION 3.16)
project(ghimo_ws)

# Set installation prefix to the common install directory
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install" CACHE PATH "Install path" FORCE)
endif()

# Add submodules
add_subdirectory(ghimo)
add_subdirectory(ghimo_python)
