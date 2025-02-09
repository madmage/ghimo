#!/bin/bash
# to install it: sudo apt install ros-noetic-stage-ros

ps -ef | grep rosmaster | grep -v grep
if [ $? -eq 1 ]; then
  echo "ROS master is not running, please start it with roscore"
else
  rosrun stage_ros stageros $(rospack find stage_ros)/world/willow-erratic.world
fi
