#!/usr/bin/env python3
import random
import time
import copy


class Environment:
    def __init__(self):
        self.agents = {}
        self.renderer = None

    def set_renderer(self, renderer):
        renderer.set_environment(self)
        self.renderer = renderer

    def step(self):
        if self.renderer:
            self.renderer.render()

    def reset(self):
        pass


class Agent:
    def __init__(self, name, definition=None):
        self.name = name
        self.definition = definition


class Renderer:
    def __init__(self):
        self.environment = None

    def set_environment(self, environment):
        self.environment = environment

    def render(self):
        pass


class EnvironmentInterface:
    def __init__(self, agent: Agent, environment: Environment = None):
        self.environment = environment
        self.agent = agent

    def set_agent(self, agent):
        self.agent = agent

    def set_environment(self, environment):
        self.environment = environment


class GridEnvironmentConsoleRenderer(Renderer):
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
                    if agent.get("goal", None) is not None and agent["goal"][0] == x and agent["goal"][1] == y:
                        print("G", end="")
                        draw_cell=False
                        break
                if draw_cell:
                    print("·" if self.environment.cells[x][y] == "." else "▮", end="")
            print("")
        print("")
        time.sleep(self.wait_time)


class GridEnvironment(Environment):
    def __init__(self):
        super().__init__()

    def random_definition(self, width, height, obstacle_probability=0.3):
        self.width = width
        self.height = height
        self.cells = []
        for x in range(width):
            row = ["X" if random.random() < obstacle_probability else "." for y in range(height)]
            self.cells.append(row)

    def load_definition(self, filename):
        pass

    def add_agent(self, agent):
        self.agents[agent.name] = {
            "state": [random.randint(0, self.width - 1), random.randint(0, self.height - 1)],
            "interface": None,
            "action": None,
        }

    def step(self):
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
            if self.cells[new_state[0]][new_state[1]] == ".":
                self.agents[agent_name]["state"] = new_state
            agent_obj["action"] = None
        super().step()


class GridEnvironmentNaiveInterface(EnvironmentInterface):
    @staticmethod
    def link(environment: Environment, agent: Agent):
        interface = GridEnvironmentNaiveInterface(environment, agent)
        interface.environment.agents[agent.name]["interface"] = interface
        interface.agent.environment_interface = interface

    def __init__(self, environment: Environment, agent: Agent):
        self.environment = environment
        self.agent = agent

    def get_observation(self):
        return self.environment.cells

    def set_action(self, action):
        self.environment.agents[agent.name]["action"] = action


class RandomAgent(Agent):
    def step(self):
        obs = self.environment_interface.get_observation()
        act = self.compute_action(obs)
        self.environment_interface.set_action(act)

    def compute_action(self, obs):
        return (random.randint(-1, 1), random.randint(-1, 1))


ENV_WIDTH = 80
ENV_HEIGHT = 40
env = GridEnvironment()
env.random_definition(ENV_WIDTH, ENV_HEIGHT, obstacle_probability=0.1)

agent = RandomAgent("agent1")
env.add_agent(agent)

GridEnvironmentNaiveInterface.link(env, agent)

renderer = GridEnvironmentConsoleRenderer(clear_screen=True, wait_time=0.1)
env.set_renderer(renderer)

env.reset()
for _ in range(100000):
    agent.step()
    env.step()

