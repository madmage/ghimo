#!/usr/bin/env python3
import random
import time
import copy

from ghimopy.environments.street_environment import StreetEnvironment, StreetEnvironmentConsoleViewer
from ghimopy.interfaces.simple_environment_agent_interface import SimpleEnvironmentAgentInterface
from ghimopy.agent.agent import Agent


class StreetEnvironmentBlindAgentInterface(SimpleEnvironmentAgentInterface):
    def __init__(self, environment: StreetEnvironment, agent_name: str):
        super().__init__(environment)
        self.agent_name = agent_name

    def observe(self):
        agent_state = self.environment.agents[self.agent_name]["state"]
        if agent_state[0] == self.environment.home:
            return ["home"] 
        elif agent_state[0] == 0 or agent_state[0] == 9:
            return ["end"]
        else:
            return ["street"]

    def act(self, action):
        self.environment.agents[self.agent_name]["action"] = action


class StreetEnvironmentAgent(Agent):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.going_forward = True

    def step(self):
        obs = self.interface.observe()
        act = self._compute_action(obs)
        self.interface.act(act)

    def _compute_action(self, obs):
        if obs[0] == "home":
            return ["enter"]
        
        if obs[0] == "end":
            self.going_forward = not self.going_forward

        if self.going_forward:
            return ["forward"]
        else:
            return ["backward"]


env = StreetEnvironment()

agent = StreetEnvironmentAgent("agent1")
env.add_agent(agent)

agent.set_interface(StreetEnvironmentBlindAgentInterface(env, agent_name="agent1"))

viewer = StreetEnvironmentConsoleViewer(wait_time=0.5)
env.set_viewer(viewer)

env.reset()
while True:
    agent.step()
    env.render()
    env.step()
