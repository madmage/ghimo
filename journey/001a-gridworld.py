#!/usr/bin/env python3
import random
import time
import copy
from ghimopy.environments.simple_environment import SimpleEnvironment

from ghimopy.agents.agent import Agent
from ghimopy.viewers.viewer import Viewer
from ghimopy.interfaces.simple_environment_agent_interface import SimpleEnvironmentAgentInterface


class GridEnvironment(SimpleEnvironment):
    def __init__(self):
        super().__init__()

    def random_definition(self, width, height, obstacle_probability=0.3):
        self.width = width
        self.height = height
        self.cells = []
        for _ in range(width):
            row = ["1" if random.random() < obstacle_probability else "0" for _ in range(height)]
            self.cells.append(row)

    def add_agent(self, agent):
        super().add_agent(agent)
        self.agents[agent.name]["initial_state"] = [random.randint(0, self.width - 1), random.randint(0, self.height - 1)]

    def reset(self):
        super().reset()

    def step(self):
        super().step()
        for agent_name, agent_obj in self.agents.items():
            action = agent_obj["action"]
            new_state = copy.deepcopy(self.agents[agent_name]["state"])
            if action[0] == 1:
                new_state[0] = min(self.agents[agent_name]["state"][0] + 1, self.width - 1)
            elif action[0] == -1:
                new_state[0] = max(self.agents[agent_name]["state"][0] - 1, 0)
            if action[1] == 1:
                new_state[1] = min(self.agents[agent_name]["state"][1] + 1, self.height - 1)
            elif action[1] == -1:
                new_state[1] = max(self.agents[agent_name]["state"][1] - 1, 0)
            if self.cells[new_state[0]][new_state[1]] == "0":
                self.agents[agent_name]["state"] = new_state
            agent_obj["action"] = None


class GridEnvironmentConsoleViewer(Viewer):
    def __init__(self, wait_time=0.0, clear_screen=False):
        self.wait_time = wait_time
        self.clear_screen = clear_screen

    def render(self):
        if self.clear_screen:
            print("\033[2J")

        for y in range(0, self.environment.height):
            for x in range(0, self.environment.width):
                draw_cell = True
                for agent_name, agent in self.environment.agents.items():
                    if agent["state"][0] == x and agent["state"][1] == y:
                        print("\033[1;96;44mA\033[0m", end="")
                        draw_cell=False
                        break
                if draw_cell:
                    print("·" if self.environment.cells[x][y] == "0" else "▮", end="")
            print("")
        print("")
        time.sleep(self.wait_time)


class EnvironmentBlindAgentInterface(SimpleEnvironmentAgentInterface):
    def __init__(self, environment: SimpleEnvironment, agent: Agent):
        self.environment = environment
        self.agent = agent

    def observe(self):
        return None

    def act(self, action):
        self.environment.agents[agent.name]["action"] = action


class GridEnvironmentRandomAgent(Agent):
    def step(self):
        obs = self.interface.observe()
        act = self._compute_action(obs)
        self.interface.act(act)

    def _compute_action(self, obs):
        return (random.randint(-1, 1), random.randint(-1, 1))


ENV_WIDTH = 80
ENV_HEIGHT = 40
env = GridEnvironment()
env.random_definition(ENV_WIDTH, ENV_HEIGHT, obstacle_probability=0.1)

agent = GridEnvironmentRandomAgent("agent1")
env.add_agent(agent)

agent.set_interface(EnvironmentBlindAgentInterface(env, agent))

viewer = GridEnvironmentConsoleViewer(clear_screen=True, wait_time=0.1)
env.set_viewer(viewer)

env.reset()
while True:
    agent.step()
    env.render()
    env.step()
