import random

from ghimo.environments.environment import Environment
from ghimo.agents.agent import Agent


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
        for p in self.agents.values():
            if p["action"] is not None:
                p["state"][0] += p["action"][0] * 0.01
                p["state"][1] += p["action"][1] * 0.01
            p["action"] = None
