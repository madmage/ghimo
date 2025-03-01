cmake_minimum_required(VERSION 2.8)
project(Ghymop_Rovina)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Qt5 packages
find_package(Qt5Widgets)
find_package(Qt5OpenGL)
find_package(Qt5Network)
find_package(Qt5X11Extras QUIET)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(../ghymop)

add_library(ghymop_rovina SHARED YarnObstAvoid_QImage.cpp)
qt5_use_modules(ghymop_rovina Widgets)
