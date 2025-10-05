#!/usr/bin/env python3
import random
from ghimopy.agents.agent import Agent

import rospy
from geometry_msgs.msg import Twist
from nav_msgs.msg import Odometry
from tf.transformations import euler_from_quaternion


class RosInterface():
    def __init__(self):
        self.odom_sub = rospy.Subscriber("/odom", Odometry, self._odom_cb)
        self.cmd_vel_pub = rospy.Publisher("/cmd_vel", Twist, queue_size=1)
        self.pose = None

    def _odom_cb(self, msg):
        _, _, yaw = euler_from_quaternion([msg.pose.pose.orientation.x, msg.pose.pose.orientation.y, msg.pose.pose.orientation.z, msg.pose.pose.orientation.w])
        self.pose = (msg.pose.pose.position.x, msg.pose.pose.position.y, yaw)

    def observe(self):
        return self.pose

    def act(self, act):
        t = Twist()
        t.linear.x = act[0]
        t.angular.z = act[1]
        self.cmd_vel_pub.publish(t)


class RosAgent(Agent):
    def step(self):
        obs = self.interface.observe()
        act = self._compute_action(obs)
        self.interface.act(act)

    def _compute_action(self, obs):
        act = (random.random() * 0.5, (random.random() - 0.5) * 2.0)
        return act


rospy.init_node("ghimo_ctrl")

agent = RosAgent("agent1")
agent.set_interface(RosInterface())

while not rospy.is_shutdown():
    agent.step()
    rospy.sleep(0.01)
