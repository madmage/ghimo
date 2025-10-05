#!/usr/bin/env python3
import math
import random

from ghimopy.environments.unicycles_environment import UnicyclesEnvironment
from ghimopy.agents.agent import Agent
from ghimopy.interfaces.simple_environment_agent_interface import SimpleEnvironmentAgentInterface
from ghimopy.viewers.unicycles_mpl_viewer import UnicyclesMplViewer
from ghimopy.geometry2d import angle_diff


class UnicycleAgent(Agent):
    def step(self):
        obs = self.interface.observe()
        if obs is None:
            act = (random.random() * 0.5, (random.random() - 0.5) * 2.0)
        else:
            act = (0.2, obs[1] * 0.2)
        self.interface.act(act)


class UnicycleEnvironmentLocalInterface(SimpleEnvironmentAgentInterface):
    def __init__(self, environment, agent):
        self.environment = environment
        self.agent = agent

    def act(self, action):
        self.environment.agents[self.agent.name]["action"] = action

    def observe(self):
        goal = self.environment.agents[self.agent.name]["goal"]
        pose = self.environment.agents[self.agent.name]["state"]
        return (
            math.hypot(goal[0] - pose[0], goal[1] - pose[1]),
            angle_diff(math.atan2(goal[1] - pose[1], goal[0] - pose[0]), pose[2])
        )


env = UnicyclesEnvironment()

agent = UnicycleAgent("unicycle")
env.add_agent(agent, initial_state=[0.0, 0.0, 0.0])
env.set_agent_goal("unicycle", [2.0, 3.0, 0.5])
UnicycleEnvironmentLocalInterface.link(env, agent)

viewer = UnicyclesMplViewer(10.0, 10.0)
env.set_viewer(viewer)

env.reset()
while not env.viewer.exit_requested:
    agent.step()
    env.render()
    env.step()
