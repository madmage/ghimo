#!/usr/bin/env python3
import math
import random
from matplotlib import pyplot as plt

from ghimo.environments.environment_base import EnvironmentBase
from ghimo.viewers.unicycle_mpl_viewer import UnicycleMplViewer


def angle_diff(a1, a2):
    return min(a1 - a2, a1 - a2 + 2 * math.pi, a1 - a2 - 2 * math.pi, key=abs)


class UnicyclePlanarEnvironment(EnvironmentBase):
    def __init__(self):
        super().__init__()
        self.viewer = UnicycleMplViewer(10.0, 10.0)

    def set_agent_goal(self, agent_name, goal):
        self.agents[agent_name]["goal"] = goal

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
        x, y, theta = self.agents[agent_name]["state"]
        gx, gy, gtheta = self.agents[agent_name]["goal"]
        dist = math.sqrt((x - gx) ** 2 + (y - gy) ** 2)
        bear = angle_diff(math.atan2(gy - y, gx - x), theta)
        return [dist, bear]

    def render(self):
        self.viewer.unicycle_pose = self.agents["unicycle"]["state"]
        self.viewer.goal_pose = self.agents["unicycle"]["goal"]
        return self.viewer.render()


class UnicycleAgent:
    def __init__(self):
        self.observation = None

    def compute_action(self):
        obs = self.observation
        if obs is None:
            return (random.random() * 0.5, (random.random() - 0.5) * 2.0)
        else:
            return (0.1, obs[1] * 0.2)

    def set_observation(self, obs):
        self.observation = obs


env = UnicyclePlanarEnvironment()
env.add_agent("unicycle", initial_state=[0, 0, 0])
env.set_agent_goal("unicycle", [2, 3, 0.5])
env.reset()

agent = UnicycleAgent()

while True:
    if not env.render():
        break

    obs = env.get_agent_observation("unicycle")
    agent.set_observation(obs)
    act = agent.compute_action()
    env.set_agent_action("unicycle", act)

    env.step()

plt.close()
