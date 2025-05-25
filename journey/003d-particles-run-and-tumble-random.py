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


class RunAndTumbleParticleAgent(Agent):
    def __init__(self, *args, **kwargs):
        super().__init__(self, *args, **kwargs)
        self.run_steps = 0

    def step(self):
        if self.run_steps == 0:
            self.run_steps = 10
            self.direction = random.uniform(0, math.pi * 2)
        act = [1.0, self.direction]
        self.run_steps -= 1
        self.interface.act(act)


env = ParticlesEnvironment(width=30.0, height=30.0)

for i in range(10):
    agent_name = f"agent{i:02d}"
    agent = RunAndTumbleParticleAgent(agent_name)
    env.add_agent(agent, initial_state=(random.uniform(-1.5, 1.5), random.uniform(-1.5, 1.5)))
    ParticlesDirectionAndSpeedInterface.link(env, agent)

viewer = ParticlesMplViewer(width=env.width, height=env.height)
env.set_viewer(viewer)

env.reset()
while not env.viewer.exit_requested:
    for agent in env.get_agents():
        agent.step()

    env.step()
