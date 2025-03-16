cmake_minimum_required(VERSION 3.15)
project(ghimo_python LANGUAGES CXX)

execute_process(
    COMMAND sh -c "python3 --version | awk '{print $2}' | awk -F. '{print $1\".\"$2}'"
    OUTPUT_VARIABLE PYTHON_COMMAND_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)

find_package(Python ${PYTHON_COMMAND_VERSION} EXACT COMPONENTS Interpreter Development REQUIRED)
message(STATUS "Building with Python version ${Python_VERSION}, current python3 version ${PYTHON_COMMAND_VERSION}")
find_package(pybind11 REQUIRED CONFIG)

string(REPLACE ":" ";" RUNTIME_PATH "$ENV{LD_LIBRARY_PATH}")
find_library(GHIMO_LIBRARY REQUIRED NAMES ghimo HINTS ${RUNTIME_PATH})
message(STATUS "Found ghimo library at ${GHIMO_LIBRARY}")
get_filename_component(GHIMO_LIBRARY_DIR ${GHIMO_LIBRARY} DIRECTORY)
find_file(GHIMO_INCLUDE_DIR REQUIRED NAMES ghimo PATHS ${GHIMO_LIBRARY_DIR}/../include)
string(REPLACE "include/ghimo" "include" GHIMO_INCLUDE_DIR ${GHIMO_INCLUDE_DIR})
message(STATUS "Ghimo include directory is: ${GHIMO_INCLUDE_DIR}")

python_add_library(ghimo_python MODULE src/ghimo_python.cpp WITH_SOABI)
target_link_libraries(ghimo_python PUBLIC pybind11::pybind11 ${GHIMO_LIBRARY})
target_include_directories(ghimo_python PUBLIC ${GHIMO_INCLUDE_DIR})

install(TARGETS ghimo_python
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/python
)
