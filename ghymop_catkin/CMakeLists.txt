cmake_minimum_required(VERSION 2.8)
project(ghymop)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/../build_tools/cmake_modules)
set(CMAKE_CXX_FLAGS "-g -ggdb")

# ROS/catkin packages
find_package(catkin REQUIRED traversability eigen_conversions) # COMPONENTS std_msgs geometry_msgs sensor_msgs tf pcl_ros tf_conversions eigen_conversions cv_bridge image_geometry)
include_directories(${catkin_INCLUDE_DIRS})
#catkin_python_setup()
catkin_package(
    INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/ghymop ${CMAKE_CURRENT_LIST_DIR}/ghymop_rovina
    LIBRARIES ghymop ghymop_rovina)

add_subdirectory(ghymop)
add_subdirectory(ghymop_rovina)
