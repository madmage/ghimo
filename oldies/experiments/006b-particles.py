#!/usr/bin/env python3
import math
from matplotlib import pyplot as plt
from ghirmo.environments.environment_base import EnvironmentBase
import random


class SimpleMplViewer:
    def __init__(self):
        self.fig = None
        self.ax = None
        plt.ion()
        self.fig, self.ax = plt.subplots()
        plt.show()

    def render(self):
        if not plt.fignum_exists(self.fig.number):
            return
        self.ax.cla()
        self.ax.grid()
        self.ax.axis("equal")
        self.ax.set_xlim([-5.0, 5.0])
        self.ax.set_ylim([-5.0, 5.0])
        self.ax.set_aspect("equal", "box")
        self.render_objects()
        plt.pause(0.01)

    def render_objects(self):
        pass


class ParticleMplViewer(SimpleMplViewer):
    def __init__(self, num_particles):
        super().__init__()
        self.particle_positions = [[0, 0] for _ in range(10)]

    def render_objects(self):
        plt.scatter(
            [e[0] for e in self.particle_positions],
            [e[1] for e in self.particle_positions]
        )


class ParticleEnvironment(EnvironmentBase):
    def __init__(self):
        super().__init__()
        self.viewer = ParticleMplViewer(10)

    def add_agent(self, name, definition, initial_state=None):
        initial_state = [0.0, 0.0]
        super().add_agent(name, definition, initial_state)

    def step(self):
        for p in self.agents.values():
            p["state"][0] += p["action"][0] * 0.01
            p["state"][1] += p["action"][1] * 0.01

    def render(self):
        for i, name in enumerate(self.agents.keys()):
            self.viewer.particle_positions[i] = self.agents[name]["state"]
        self.viewer.render()


class ParticleAgent:
    def __init__(self):
        self.observation = None

    def set_observation(self, observation):
        self.observation = observation

    def compute_action(self):
        return [random.random() * 2.0 - 1.0, random.random() * 2.0 - 1.0]


env = ParticleEnvironment()
agents = {}
for i in range(10):
    agent_name = f"agent{i:02d}"
    agents[agent_name] = ParticleAgent()
    env.add_agent(agent_name, agents[agent_name])

env.reset()

for _ in range(10000):
    env.render()
    for i in range(10):
        agent_name = f"agent{i:02d}"
        agent = agents[agent_name]
        obs = env.get_agent_observation(agent_name)
        agent.set_observation(obs)
        act = agent.compute_action()
        env.set_agent_action(agent_name, act)
    env.step()
    #feedback = env.get_feedback_for_agent("ecoli")
    #agent.process_feedback(feedback)
    #if env.agent_has_finished("ecoli"):
    #    env.render()
    #    break

plt.close()
