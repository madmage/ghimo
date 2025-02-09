#!/usr/bin/env python3
import math
import random

from ghimo.environments.environment import Environment
from ghimo.agent import Agent
from ghimo.viewers.unicycle_mpl_viewer import UnicycleMplViewer


class UnicyclePlanarEnvironment(Environment):
    def __init__(self):
        super().__init__()
        self.viewer = UnicycleMplViewer(10.0, 10.0)

    def step(self):
        dt = 0.1
        for agent_name, agent in self.agents.items():
            if "action" in agent:
                v, w = agent["action"]
                x, y, theta = agent["state"]
                xx = x + math.cos(theta) * v * dt
                yy = y + math.sin(theta) * v * dt
                ttheta = theta + w * dt
                agent["state"] = xx, yy, ttheta

    def get_agent_observation(self, agent_name):
        return None

    def render(self):
        self.viewer.unicycle_pose = self.agents["unicycle"]["state"]
        return self.viewer.render()

