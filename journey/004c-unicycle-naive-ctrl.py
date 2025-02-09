#!/usr/bin/env python3
import math
import random
from matplotlib import pyplot as plt

from ghimo.environments.environment import Environment, EnvironmentInterface
from ghimo.viewers.unicycle_mpl_viewer import UnicycleMplViewer
from ghimo.agent import Agent


def angle_diff(a1, a2):
    return min(a1 - a2, a1 - a2 + 2 * math.pi, a1 - a2 - 2 * math.pi, key=abs)


class UnicyclePlanarEnvironment(Environment):
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


class UnicycleAgent(Agent):
    def step(self):
        obs = self.environment_interface.get_observation()
        if obs is None:
            act = (random.random() * 0.5, (random.random() - 0.5) * 2.0)
        else:
            act = (0.1, obs[1] * 0.2)
        self.environment_interface.set_action(act)


def ang_diff(a0, a1):
    return math.atan2(math.sin(a0 - a1), math.cos(a0 - a1))


class EnvironmentNaiveInterface(EnvironmentInterface):
    def __init__(self, environment, agent):
        self.environment = environment
        self.agent = agent

    def set_action(self, action):
        self.environment.agents[self.agent.name]["action"] = action

    def get_observation(self):
        goal = self.environment.agents[self.agent.name]["goal"]
        pose = self.environment.agents[self.agent.name]["state"]
        return (
            math.hypot(goal[0] - pose[0], goal[1] - pose[1]),
            ang_diff(math.atan2(goal[1] - pose[1], goal[0] - pose[0]), pose[2])
        )


env = UnicyclePlanarEnvironment()
agent = UnicycleAgent("unicycle")

env.add_agent(agent, initial_state=[0, 0, 0])
env.set_agent_goal("unicycle", [2, 3, 0.5])
env.reset()

EnvironmentNaiveInterface.link(env, agent)

while True:
    if not env.render():
        break

    agent.step()
    env.step()

