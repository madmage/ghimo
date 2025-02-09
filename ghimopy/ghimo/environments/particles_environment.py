from typing import Any
import random

from ghimo.environments.environment import Environment, EnvironmentInterface
from ghimo.agent import Agent
from ghimo.viewers.particles_mpl_viewer import ParticlesMplViewer


class ParticlesEnvironment(Environment):
    def __init__(self, width, height):
        super().__init__()
        self.width = width
        self.height = height
        self.viewer = ParticlesMplViewer(width=width, height=height)

    def add_agent(self, agent : Agent, initial_state=None) -> None:
        state = [random.random() * 0.1 - 0.05, random.random() * 0.1 - 0.05]
        super().add_agent(agent, initial_state=state)

    def step(self) -> None:
        pass

    def render(self) -> None:
        self.viewer.particle_positions = []
        for i, name in enumerate(self.agents.keys()):
            self.viewer.particle_positions.append(self.agents[name]["state"])
        return self.viewer.render()


class ParticlesEnvironmentNaiveInterface(EnvironmentInterface):
    def get_observation(self):
        return None

    def set_action(self, action):
        self.environment.agents[self.agent.name]["action"] = action
