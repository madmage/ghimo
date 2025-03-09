#!/usr/bin/env python3
import math
import random

from ghimo.environments.environment import Environment
from ghimo.interfaces.environment_blind_agent_interface import EnvironmentBlindAgentInterface
from ghimo.viewers.particles_mpl_viewer import ParticlesMplViewer
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
        dt = 0.1
        for p in self.agents.values():
            if p["action"] is not None:
                p["state"][0] += p["action"][0] * dt
                p["state"][1] += p["action"][1] * dt
            p["action"] = None


class RandomParticleAgent(Agent):
    def step(self):
        obs = self.interface.get_observation()
        act = self._compute_action(obs)
        self.interface.set_action(act)

    def _compute_action(self, obs):
        return [random.uniform(-1.0, 1.0), random.uniform(-1.0, 1.0)]


env = ParticlesEnvironment(width=30.0, height=30.0)

for i in range(10):
    agent_name = f"agent{i:02d}"
    agent = RandomParticleAgent(agent_name)
    env.add_agent(agent, initial_state=(random.uniform(-0.1, 0.1), random.uniform(-0.1, 0.1)))
    EnvironmentBlindAgentInterface.link(env, agent)

viewer = ParticlesMplViewer(width=env.width, height=env.height)
env.set_viewer(viewer)

env.reset()

while not env.viewer.exit_requested:
    for agent in env.get_agents():
        agent.step()

    env.step()
