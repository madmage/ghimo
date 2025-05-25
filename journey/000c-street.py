#!/usr/bin/env python3
import random
import time
import copy


class Environment:
    def __init__(self):
        self.agents = {}
        self.viewer = None

    def set_viewer(self, viewer):
        viewer.set_environment(self)
        self.viewer = viewer

    def add_agent(self, agent):
        self.agents[agent.name] = agent

    def render(self):
        if self.viewer:
            self.viewer.render()

    def step(self):
        pass

    def reset(self):
        pass


class Agent:
    def __init__(self, name, definition=None):
        self.name = name
        self.definition = definition
        self.interface = None

    def step(self):
        pass


class Viewer:
    def __init__(self):
        self.environment = None

    def set_environment(self, environment):
        self.environment = environment

    def render(self):
        pass


class EnvironmentAgentInterface:
    @classmethod
    def link(cls, environment: Environment, agent: Agent) -> None:
        interface = cls(environment, agent)
        interface.agent.interface = interface

    def __init__(self, environment: Environment = None, agent: Agent = None):
        self.environment = environment
        self.agent = agent

    def observe(self):
        pass

    def act(self, action):
        pass


class StreetEnvironment(Environment):
    def __init__(self):
        super().__init__()
        self.home = 7

    def add_agent(self, agent):
        super().add_agent(agent)
        self.agents[agent.name] = {
            "state": [random.randint(0, 10)],
            "action": None,
        }

    def step(self):
        super().step()
        for agent_name, agent_dict in self.agents.items():
            action = agent_dict["action"]
            new_state = copy.deepcopy(self.agents[agent_name]["state"])
            if action[0] == "forward":
                new_state[0] = min(new_state[0] + 1, 9)
            elif action[0] == "backward":
                new_state[0] = max(0, new_state[0] - 1)
            agent_dict["action"] = None
            agent_dict["state"] = new_state


class StreetEnvironmentConsoleViewer(Viewer):
    def __init__(self, wait_time=1.0):
        self.wait_time = wait_time

    def render(self):
        for agent_name, agent in self.environment.agents.items():
            print(f"Agent {agent_name} is in place {agent['state'][0]}, home is in place {self.environment.home}, agent choose as action {agent['action'][0]}")
        print("")
        time.sleep(self.wait_time)


class StreetEnvironmentAgentInterface(EnvironmentAgentInterface):
    def observe(self):
        return ["home"] if self.environment.agents[agent.name]["state"][0] == self.environment.home else ["street"]

    def act(self, action):
        self.environment.agents[agent.name]["action"] = action


class StreetEnvironmentAgent(Agent):
    def step(self):
        obs = self.interface.observe()
        act = self._compute_action(obs)
        self.interface.act(act)

    def _compute_action(self, obs):
        if obs[0] == "home":
            return ["enter"]
        else:
            return ["forward"] if random.randint(0, 2) == 0 else ["backward"]


env = StreetEnvironment()

agent = StreetEnvironmentAgent("agent1")
env.add_agent(agent)
StreetEnvironmentAgentInterface.link(env, agent)

viewer = StreetEnvironmentConsoleViewer(wait_time=0.5)
env.set_viewer(viewer)

env.reset()
while True:
    agent.step()
    env.render()
    env.step()
