#!/usr/bin/env python3
import math
from matplotlib import pyplot as plt
from ghimo.environments.particles_environment import ParticlesEnvironment
import random


class MyParticlesEnvironment(ParticlesEnvironment):
    def step(self):
        for p in self.agents.values():
            p["state"][0] += p["action"][0] * 0.01
            p["state"][1] += p["action"][1] * 0.01

    def get_agent_observation(self, agent_name):
        agent = self.agents[agent_name]
        min_dist = None
        min_agent = None
        for name, other in self.agents.items():
            if name == agent_name:
                continue
            dist = math.hypot(agent["state"][0] - other["state"][0], agent["state"][1] - other["state"][1])
            if min_dist is None or dist < min_dist:
                min_dist = dist
                min_agent = other
        return [agent["state"][0] - min_agent["state"][0], agent["state"][1] - min_agent["state"][1]]


class ParticleAgent:
    def __init__(self):
        self.observation = None

    def set_observation(self, observation):
        self.observation = observation

    def _dist_to_act(self, dist):
        a = abs(dist) - 1.0
        if a < -1.0:
            a = 0.0
        elif a > 1.0:
            a = 0.0
        return a * (-1 if dist > 0 else 1)

    def compute_action(self):
        noise = [random.random() * 2.0 - 1.0, random.random() * 2.0 - 1.0]
        act = [self._dist_to_act(self.observation[0]), self._dist_to_act(self.observation[1])]
        return [
            act[0] * 50.0 + noise[0],
            act[1] * 50.0 + noise[1]
        ]


env = MyParticlesEnvironment(width=10.0, height=10.0)
agents = {}
for i in range(10):
    agent_name = f"agent{i:02d}"
    agents[agent_name] = ParticleAgent()
    env.add_agent(agent_name) #, agents[agent_name])

env.reset()

while True:
    if not env.render():
        break

    for i in range(10):
        agent_name = f"agent{i:02d}"
        agent = agents[agent_name]
        obs = env.get_agent_observation(agent_name)
        agent.set_observation(obs)
        act = agent.compute_action()
        env.set_agent_action(agent_name, act)

    env.step()

plt.close()
