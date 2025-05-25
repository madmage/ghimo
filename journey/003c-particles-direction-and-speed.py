#!/usr/bin/env python3
import math
import random

from ghimopy.environments.particles_environment import ParticlesEnvironment
from ghimopy.interfaces.environment_blind_agent_interface import EnvironmentBlindAgentInterface
from ghimopy.viewers.particles_mpl_viewer import ParticlesMplViewer
from ghimopy.agents.agent import Agent


class ParticlesDirectionAndSpeedInterface(EnvironmentBlindAgentInterface):
    def act(self, action):
        self.environment.agents[self.agent.name]["action"] = [action[0] * math.cos(action[1]), action[0] * math.sin(action[1])]

    def observe(self):
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
        direction = math.atan2(min_agent["state"][1] - agent["state"][1], min_agent["state"][0] - agent["state"][0])
        return [min_dist, direction]


class ParticleAgent(Agent):
    def _dist_to_act(self, dist):
        a = abs(dist) - 1.0
        if a < -1.0:
            a = 0.0
        elif a > 1.0:
            a = 0.0
        return a * (-1 if dist > 0 else 1)

    def step(self):
        obs = self.interface.observe()
        act = self._compute_action(obs)
        self.interface.act(act)

    def _compute_action(self, obs):
        speed = self._dist_to_act(obs[0])
        direction = -obs[1]
        return [speed, direction]


env = ParticlesEnvironment(width=30.0, height=30.0)

for i in range(10):
    agent_name = f"agent{i:02d}"
    agent = ParticleAgent(agent_name)
    env.add_agent(agent, initial_state=(random.uniform(-1.5, 1.5), random.uniform(-1.5, 1.5)))
    ParticlesDirectionAndSpeedInterface.link(env, agent)

viewer = ParticlesMplViewer(width=env.width, height=env.height)
env.set_viewer(viewer)

env.reset()
while not env.viewer.exit_requested:
    for agent in env.get_agents():
        agent.step()

    env.step()
