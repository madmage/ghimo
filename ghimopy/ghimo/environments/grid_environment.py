import random
import copy

from ghimo.environments.environment import Environment
from ghimo.agents.agent import Agent

# in a GridEnvironment:
# - a state is a list [x, y]
# - an action is a list [dx, dy] where dx and dy can be 0, -1 or 1
# - a goal is like a state
class GridEnvironment(Environment):
    def __init__(self):
        super().__init__()

    def random_definition(self, width, height, obstacle_probability=0.3) -> None:
        self.width = width
        self.height = height
        self.cells = []
        for _ in range(width):
            row = ["1" if random.random() < obstacle_probability else "0" for _ in range(height)]
            self.cells.append(row)

    def add_agent(self, agent: Agent, initial_state=None) -> None:
        if initial_state is None:
            initial_state = [random.randint(0, self.width - 1), random.randint(0, self.height - 1)]
        super().add_agent(agent, initial_state=initial_state)
        self.agents[agent.name]["goal"] = None

    def set_agent_goal(self, agent_name, goal):
        self.agents[agent_name]["goal"] = goal

    def step(self) -> None:
        super().step()  # must be called before modifying the current state
        for agent_name, agent_dict in self.agents.items():
            action = agent_dict["action"]
            if action is None:
                return
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
            agent_dict["action"] = None

