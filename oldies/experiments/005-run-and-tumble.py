#!/usr/bin/env python3
import math
from matplotlib import pyplot as plt
import matplotlib as mpl
from ghirmo.environments.environment_base import EnvironmentBase
from typing import Tuple
import random


class EColiEnvironment(EnvironmentBase):
    def __init__(self):
        self.agents = {}
        self.fig = None
        self.ax = None
        self.mpl_ecoli = mpl.patches.PathPatch(mpl.path.Path([(0.2, 0.0), (0.0, 0.1), (0.0, -0.1), (0.2, 0.0)]), facecolor="red", edgecolor="black")

    def add_agent(self, name: str, initial_state):
        self.agents[name] = {"state": initial_state}

    def reset(self):
        plt.ion()
        self.fig, self.ax = plt.subplots()
        plt.show()

    def render(self):
        self.ax.cla()
        self.ax.grid()
        self.ax.axis("equal")
        self.ax.set_xlim([-5.0, 5.0])
        self.ax.set_ylim([-5.0, 5.0])
        self.ax.set_aspect("equal", "box")
        for agent_name, agent in self.agents.items():
            x, y, theta = agent["state"]
            self.mpl_ecoli.set_transform(mpl.transforms.Affine2D().rotate(theta).translate(x, y) + self.ax.transData)
            self.ax.add_patch(self.mpl_ecoli)
        plt.pause(0.01)

    def set_agent_action(self, agent_name: str, action):
        self.agents[agent_name]["action"] = action

    def set_agent_goal(self, agent_name: str, goal):
        pass

    def get_agent_observation(self, agent_name: str):
        pass

    def get_feedback_for_agent(self, agent_name: str) -> Tuple[float, bool]:
        pass

    def agent_has_finished(self, agent_name) -> bool:
        return False

    def step(self):
        for agent_name, agent in self.agents.items():
            if "action" in agent:
                act = agent["action"]
                if act[0] == 1:
                    agent["state"][0] += 0.1 * math.cos(agent["state"][2])
                    agent["state"][1] += 0.1 * math.sin(agent["state"][2])
                    if agent["state"][0] > 5.0:
                        agent["state"][0] -= 5.0 - 5.0
                    elif agent["state"][0] < -5.0:
                        agent["state"][0] += 5.0 + 5.0
                    if agent["state"][1] > 5.0:
                        agent["state"][1] -= 5.0 - 5.0
                    elif agent["state"][1] < -5.0:
                        agent["state"][1] += 5.0 + 5.0
                elif act[1] == 1:
                    agent["state"][2] += 0.1
                elif act[1] == -1:
                    agent["state"][2] += -0.1


class EColiAgent:
    def __init__(self):
        self.tumbling = 0

    def compute_action(self, obs):
        if self.tumbling != 0:
            if random.random() < 0.1:
                self.tumbling = 0
                return (1, 0)
            else:
                return (0, self.tumbling)
        else:
            if random.random() < 0.2:
                if random.random() < 0.5:
                    self.tumbling = 1
                else:
                    self.tumbling = -1
                return (0, self.tumbling)
            else:
                return (1, 0)

    def process_feedback(self, feedback):
        pass


env = EColiEnvironment()
agent = EColiAgent()
env.add_agent("ecoli", [0, 0, 0])
env.reset()

for _ in range(10000):
    env.render()
    obs = env.get_agent_observation("ecoli")
    act = agent.compute_action(obs)
    env.set_agent_action("ecoli", act)
    env.step()
    feedback = env.get_feedback_for_agent("ecoli")
    agent.process_feedback(feedback)
    if env.agent_has_finished("ecoli"):
        env.render()
        break

plt.close()
