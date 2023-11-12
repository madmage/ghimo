from typing import Any
import random
from ghimo.environments.environment_base import EnvironmentBase
from ghimo.viewers.particles_mpl_viewer import ParticlesMplViewer


class ParticlesEnvironment(EnvironmentBase):
    def __init__(self, width, height):
        super().__init__()
        self.width = width
        self.height = height
        self.viewer = ParticlesMplViewer(width=width, height=height)

    def add_agent(self, name, definition=None, initial_state=None) -> None:
        initial_state = [random.random() * 0.1 - 0.05, random.random() * 0.1 - 0.05]
        super().add_agent(name, definition, initial_state)

    def step(self) -> None:
        pass

    def get_agent_observation(self, agent_name) -> Any:
        pass

    def render(self) -> None:
        self.viewer.particle_positions = []
        for i, name in enumerate(self.agents.keys()):
            self.viewer.particle_positions.append(self.agents[name]["state"])
        return self.viewer.render()

