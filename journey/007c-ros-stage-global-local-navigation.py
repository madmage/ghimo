#!/usr/bin/env python3
import math
import random

import rospy
from geometry_msgs.msg import Twist, PoseStamped
from nav_msgs.msg import Odometry
from tf.transformations import euler_from_quaternion, quaternion_from_euler

from ghimopy.agents.agent import Agent
from ghimopy.geometry2d import angle_diff, pose_is_close
from ghimopy.control2d import polar_coordinates_ctrl


class RosInterface():
    def __init__(self):
        self.odom_sub = rospy.Subscriber("/odom", Odometry, self._odom_cb)
        self.goal_sub = rospy.Subscriber("/move_base_simple/goal", PoseStamped, self._goal_cb)
        self.cmd_vel_pub = rospy.Publisher("/cmd_vel", Twist, queue_size=1)
        self.pose = None
        self.goal = None

    def _odom_cb(self, msg):
        _, _, yaw = euler_from_quaternion([msg.pose.pose.orientation.x, msg.pose.pose.orientation.y, msg.pose.pose.orientation.z, msg.pose.pose.orientation.w])
        self.pose = (msg.pose.pose.position.x, msg.pose.pose.position.y, yaw)

    def _goal_cb(self, msg):
        _, _, yaw = euler_from_quaternion([msg.pose.orientation.x, msg.pose.orientation.y, msg.pose.orientation.z, msg.pose.orientation.w])
        self.goal = (msg.pose.position.x, msg.pose.position.y, yaw)

    def observe(self):
        pose = self.pose
        goal = self.goal
        if not pose or not goal:
            return None
        else:
            return (
                math.hypot(goal[0] - pose[0], goal[1] - pose[1]),
                angle_diff(math.atan2(goal[1] - pose[1], goal[0] - pose[0]), pose[2]),
                angle_diff(pose[2], goal[2])
            )

    def act(self, act):
        t = Twist()
        t.linear.x = act[0]
        t.angular.z = act[1]
        self.cmd_vel_pub.publish(t)


class RosAgentPoseStabilization(Agent):
    def step(self):
        obs = self.interface.observe()
        if obs:
            rho, gamma, delta = obs
            v, w = polar_coordinates_ctrl(rho, gamma, delta)
            self.interface.act((v, w))


rospy.init_node("ghimo_ctrl")

agent = RosAgentPoseStabilization("agent1")
agent.set_interface(RosInterface())

goal_pub = rospy.Publisher("/move_base_simple/goal", PoseStamped, queue_size=1)

while not rospy.is_shutdown():
    agent.step()

    if (not agent.interface.goal and agent.interface.pose) or (agent.interface.pose and agent.interface.goal and pose_is_close(agent.interface.pose, agent.interface.goal)):
        goal = (
            agent.interface.pose[0] + random.uniform(-4.0, 4.0),
            agent.interface.pose[1] + random.uniform(-4.0, 4.0),
            random.uniform(0.0, math.pi * 2.0),
        )
        print(f"Goal: {goal}")
        goal_ros = PoseStamped()
        goal_ros.header.stamp = rospy.get_rostime()
        goal_ros.header.frame_id = "odom"
        goal_ros.pose.position.x = goal[0]
        goal_ros.pose.position.y = goal[1]
        goal_ros.pose.position.z = 0.0
        quat = quaternion_from_euler(0.0, 0.0, goal[2])
        goal_ros.pose.orientation.x = quat[0]
        goal_ros.pose.orientation.y = quat[1]
        goal_ros.pose.orientation.z = quat[2]
        goal_ros.pose.orientation.w = quat[3]
        goal_pub.publish(goal_ros)

    rospy.sleep(0.01)
