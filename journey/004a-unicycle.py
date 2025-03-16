#!/usr/bin/env python3
import math
import random

from ghimopy.environments.environment import Environment
from ghimopy.agents.agent import Agent
from ghimopy.interfaces.environment_blind_agent_interface import EnvironmentBlindAgentInterface
from ghimopy.viewers.unicycles_mpl_viewer import UnicyclesMplViewer

# in a UnicyclesEnvironment:
# - a state is a list [x, y, theta], where theta is the orientation of the unicycle
# - an action is a list [speed, job]
class UnicycleEnvironment(Environment):
    def step(self):
        super().step()
        dt = 0.1
        for agent in self.agents.values():
            if "action" in agent:
                x0, y0, theta0 = agent["state"]
                v0, w0 = agent["action"]
                x1 = x0 + math.cos(theta0) * v0 * dt
                y1 = y0 + math.sin(theta0) * v0 * dt
                theta1 = theta0 + w0 * dt
                agent["state"] = x1, y1, theta1


class RandomUnicycleAgent(Agent):
    def step(self):
        act = (random.random() * 0.5, (random.random() - 0.5) * 2.0)
        self.interface.set_action(act)


env = UnicycleEnvironment()

agent = RandomUnicycleAgent("unicycle")
env.add_agent(agent, initial_state=[0.0, 0.0, 0.0])
EnvironmentBlindAgentInterface.link(env, agent)

viewer = UnicyclesMplViewer(10.0, 10.0)
env.set_viewer(viewer)

env.reset()

while not env.viewer.exit_requested:
    agent.step()
    env.step()
