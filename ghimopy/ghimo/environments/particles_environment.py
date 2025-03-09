import random

from ghimo.environments.environment import Environment
from ghimo.agents.agent import Agent

# in a ParticlesEnvironment:
# - a state is a list [x, y] where x and y are arbitrary floats
# - an action is a list [dx, dy] where dx and dy are arbitrary floats
class ParticlesEnvironment(Environment):
    def __init__(self, width, height):
        super().__init__()
        self.width = width
        self.height = height

    def add_agent(self, agent: Agent, initial_state=None) -> None:
        if initial_state is None:
            initial_state = [random.uniform(-0.05, 0.05), random.uniform(-0.05, 0.05)]
        super().add_agent(agent, initial_state=list(initial_state))

    def step(self) -> None:
        super().step()
        dt = 0.1
        for p in self.agents.values():
            if p["action"] is not None:
                p["state"][0] += p["action"][0] * dt
                p["state"][1] += p["action"][1] * dt
            p["action"] = None
