import random
import copy

from ghimo.environments.environment import Environment, EnvironmentInterface
from ghimo.agent import Agent


class GridEnvironment(Environment):
    def __init__(self):
        super().__init__()

    def random_definition(self, width, height, obstacle_probability=0.3) -> None:
        self.width = width
        self.height = height
        self.cells = []
        for x in range(width):
            row = ["X" if random.random() < obstacle_probability else "." for y in range(height)]
            self.cells.append(row)

    def load_definition(self, filename) -> None:
        pass

    def add_agent(self, agent: Agent, initial_state=None) -> None:
        super().add_agent(agent, initial_state=[random.randint(0, self.width - 1), random.randint(0, self.height - 1)])

    def step(self) -> None:
        super().step()  # must be called before modifying the current state
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


class GridEnvironmentNaiveInterface(EnvironmentInterface):
    def get_observation(self):
        return {
            "map": self.environment.cells,
            "agent_position": self.environment.agents[self.agent.name]["state"],
        }

    def set_action(self, action):
        self.environment.agents[self.agent.name]["action"] = action

    def set_agent_info(self, agent_info):
        self.environment.agents[self.agent.name]["info"] = agent_info
