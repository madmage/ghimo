#!/usr/bin/env python3
import math
from matplotlib import pyplot as plt
from ghimo.environments.particles_environment import ParticlesEnvironment, ParticlesEnvironmentNaiveInterface
from ghimo.agent import Agent
import random


class MyParticlesEnvironment(ParticlesEnvironment):
    def __init__(self, width, height):
        super().__init__(width, height)

    def step(self):
        for p in self.agents.values():
            p["state"][0] += p["action"][0] * 0.01
            p["state"][1] += p["action"][1] * 0.01


class MyParticlesEnvironmentNaiveInterface(ParticlesEnvironmentNaiveInterface):
    def get_observation(self):
        agent = self.environment.agents[agent_name]
        min_dist = None
        min_agent = None
        for name, other in self.environment.agents.items():
            if name == agent_name:
                continue
            dist = math.hypot(agent["state"][0] - other["state"][0], agent["state"][1] - other["state"][1])
            if min_dist is None or dist < min_dist:
                min_dist = dist
                min_agent = other
        return [agent["state"][0] - min_agent["state"][0], agent["state"][1] - min_agent["state"][1]]


class ParticleAgent(Agent):
    def __init__(self, name):
        super().__init__(name)

    def _dist_to_act(self, dist):
        a = abs(dist) - 1.0
        if a < -1.0:
            a = 0.0
        elif a > 1.0:
            a = 0.0
        return a * (-1 if dist > 0 else 1)

    def step(self):
        obs = self.environment_interface.get_observation()
        act = self.compute_action(obs)
        self.environment_interface.set_action(act)
        
    def compute_action(self, observation):
        noise = [random.random() * 2.0 - 1.0, random.random() * 2.0 - 1.0]
        act = [self._dist_to_act(observation[0]), self._dist_to_act(observation[1])]
        return [
            act[0] * 50.0 + noise[0],
            act[1] * 50.0 + noise[1]
        ]


env = MyParticlesEnvironment(width=10.0, height=10.0)
agents = {}
for i in range(10):
    agent_name = f"agent{i:02d}"
    agents[agent_name] = ParticleAgent(agent_name)
    env.add_agent(agents[agent_name])
    MyParticlesEnvironmentNaiveInterface.link(env, agents[agent_name])

env.reset()

while True:
    if not env.render():
        break

    for i in range(10):
        agent_name = f"agent{i:02d}"
        agent = agents[agent_name]
        agent.step()

    env.step()

plt.close()
