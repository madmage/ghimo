#!/usr/bin/env python3
import math
import random

from ghimo.environments.particles_environment import ParticlesEnvironment
from ghimo.interfaces.environment_blind_agent_interface import EnvironmentBlindAgentInterface
from ghimo.viewers.particles_mpl_viewer import ParticlesMplViewer
from ghimo.agents.agent import Agent


class ParticlesClosestDistanceInterface(EnvironmentBlindAgentInterface):
    def get_observation(self):
        agent = self.environment.agents[self.agent.name]
        min_dist = None
        min_agent = None
        for name, other in self.environment.agents.items():
            if name == self.agent.name:
                continue
            dist = math.hypot(agent["state"][0] - other["state"][0], agent["state"][1] - other["state"][1])
            if min_dist is None or dist < min_dist:
                min_dist = dist
                min_agent = other
        return [agent["state"][0] - min_agent["state"][0], agent["state"][1] - min_agent["state"][1]]


class ParticleAgent(Agent):
    def _dist_to_act(self, dist):
        a = abs(dist) - 1.0
        if a < -1.0:
            a = 0.0
        elif a > 1.0:
            a = 0.0
        return a * (-1 if dist > 0 else 1)

    def step(self):
        obs = self.interface.get_observation()
        act = self._compute_action(obs)
        self.interface.set_action(act)

    def _compute_action(self, obs):
        noise = [random.uniform(-1.0, 1.0), random.uniform(-1.0, 1.0)]
        act = [self._dist_to_act(obs[0]), self._dist_to_act(obs[1])]
        return [
            act[0] * 10.0 + noise[0],
            act[1] * 10.0 + noise[1]
        ]


env = ParticlesEnvironment(width=30.0, height=30.0)

for i in range(10):
    agent_name = f"agent{i:02d}"
    agent = ParticleAgent(agent_name)
    env.add_agent(agent, initial_state=(random.uniform(-0.1, 0.1), random.uniform(-0.1, 0.1)))
    ParticlesClosestDistanceInterface.link(env, agent)

viewer = ParticlesMplViewer(width=env.width, height=env.height)
env.set_viewer(viewer)

env.reset()

while not env.viewer.exit_requested:
    for agent in env.get_agents():
        agent.step()

    env.step()
