#!/usr/bin/env python3
import math
import random

from ghimo.environments.particles_environment import ParticlesEnvironment
from ghimo.interfaces.environment_blind_agent_interface import EnvironmentBlindAgentInterface
from ghimo.viewers.particles_mpl_viewer import ParticlesMplViewer
from ghimo.agents.agent import Agent


class ParticlesDirectionAndSpeedInterface(EnvironmentBlindAgentInterface):
    def set_action(self, action):
        self.environment.agents[self.agent.name]["action"] = [action[0] * math.cos(action[1]), action[0] * math.sin(action[1])]

    def get_observation(self):
        agent_position = self.environment.agents[self.agent.name]["state"]
        return [-math.hypot(agent_position[0] - 0.0, agent_position[1] - 10.0)]


class RunAndTumbleParticleAgent(Agent):
    def __init__(self, *args, **kwargs):
        super().__init__(self, *args, **kwargs)
        self.run_steps = 0
        self.last_obs = None

    def step(self):
        if self.run_steps == 0:
            self.run_steps = 3
            if self.last_obs is None:
                self.direction = random.uniform(0, math.pi * 2)
                self.last_obs = self.interface.get_observation()[0]
            else:
                obs = self.interface.get_observation()[0]
                delta_obs = obs - self.last_obs
                if delta_obs > 0:
                    a = 0.1
                else:
                    a = 2.5
                self.direction = self.direction + random.uniform(-1.0, 1.0) * a
                self.last_obs = obs
        act = [1.0, self.direction]
        self.run_steps -= 1
        self.interface.set_action(act)


env = ParticlesEnvironment(width=30.0, height=30.0)

for i in range(10):
    agent_name = f"agent{i:02d}"
    agent = RunAndTumbleParticleAgent(agent_name)
    env.add_agent(agent, initial_state=(random.uniform(-10.0, 10.0), random.uniform(-10.0, 10.0)))
    ParticlesDirectionAndSpeedInterface.link(env, agent)

viewer = ParticlesMplViewer(width=env.width, height=env.height)
env.set_viewer(viewer)

env.reset()

while not env.viewer.exit_requested:
    for agent in env.get_agents():
        agent.step()

    env.step()
