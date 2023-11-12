#!/usr/bin/env python3
import math
from matplotlib import pyplot as plt
import matplotlib as mpl
from ghirmo.environments.environment_base import EnvironmentBase
from typing import Tuple
import random


class PlanarEnvironment(EnvironmentBase):
    def __init__(self):
        self.agents = {}
        # gfx
        self.fig = None
        self.ax = None
        self.mpl_triangle = mpl.patches.PathPatch(mpl.path.Path([(0.5, 0.0), (0.0, 0.2), (0.0, -0.2), (0.5, 0.0)]), facecolor="red", edgecolor="black")

    def add_agent(self, name: str, initial_state):
        self.agents[name] = {"state": initial_state}

    def reset(self):
        # gfx
        plt.ion()
        self.fig, self.ax = plt.subplots()
        plt.show()

    def render(self):
        # gfx
        self.ax.cla()
        self.ax.grid()
        self.ax.axis("equal")
        self.ax.set_xlim([-5.0, 5.0])
        self.ax.set_ylim([-5.0, 5.0])
        self.ax.set_aspect("equal", "box")
        for agent_name, agent in self.agents.items():
            x, y, theta = agent["state"]
            self.mpl_triangle.set_transform(mpl.transforms.Affine2D().rotate(theta).translate(x, y) + self.ax.transData)
            self.ax.add_patch(self.mpl_triangle)
        plt.pause(0.01)

    def set_agent_action(self, agent_name: str, action):
        self.agents[agent_name]["action"] = action

    def set_agent_goal(self, agent_name: str, goal):
        pass

    def get_agent_observation(self, agent_name: str):
        pass

    def get_feedback_for_agent(self, agent_name: str) -> Tuple[float, bool]:
        pass

        return False

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


class UnicycleAgent:
    def __init__(self):
        self.tumbling = 0

    def compute_action(self, obs):
        return (random.random() * 0.5, (random.random() - 0.5) * 2.0)

    def process_feedback(self, feedback):
        pass


env = PlanarEnvironment()
agent = UnicycleAgent()
env.add_agent("unicycle", [0, 0, 0])
env.reset()

for _ in range(10000):
    env.render()
    obs = env.get_agent_observation("unicycle")
    act = agent.compute_action(obs)
    env.set_agent_action("unicycle", act)
    env.step()
    feedback = env.get_feedback_for_agent("unicycle")
    agent.process_feedback(feedback)
    if env.agent_has_finished("unicycle"):
        env.render()
        break

plt.close()
