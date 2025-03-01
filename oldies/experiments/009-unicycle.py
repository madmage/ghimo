#!/usr/bin/env python3
import math
import random
from matplotlib import pyplot as plt

from ghimo.environments.environment_base import EnvironmentBase
from ghimo.viewers.unicycles_mpl_viewer import UnicycleMplViewer


class UnicyclePlanarEnvironment(EnvironmentBase):
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


class UnicycleAgent:
    def compute_action(self):
        return (random.random() * 0.5, (random.random() - 0.5) * 2.0)

    def set_observation(self, obs):
        pass


env = UnicyclePlanarEnvironment()
env.add_agent("unicycle", initial_state=[0, 0, 0])
env.reset()

agent = UnicycleAgent()

while True:
    if not env.render():
        break

    obs = env.get_agent_observation("unycicle")
    agent.set_observation(obs)
    act = agent.compute_action()
    env.set_agent_action("unicycle", act)

    env.step()

plt.close()
