#!/usr/bin/env python3
import math
from matplotlib import pyplot as plt
from ghirmo.environments.environment_base import EnvironmentBase
import random


class SimpleMplViewer:
    def __init__(self, width, height):
        plt.ion()
        self._fig, self._ax = plt.subplots()
        plt.show()
        self._width = width
        self._height = height

    def render(self):
        if not plt.fignum_exists(self.fig.number):
            return
        self._ax.cla()
        self._ax.grid()
        self._ax.axis("equal")
        self._ax.set_xlim([-self._width / 2.0, self._width / 2.0])
        self._ax.set_ylim([-self._height / 2.0, self._height / 2.0])
        self._ax.set_aspect("equal", "box")
        self.render_objects()
        plt.pause(0.01)

    def render_objects(self):
        pass


class ParticleMplViewer(SimpleMplViewer):
    def __init__(self, width, height, num_particles):
        super().__init__(width, height)
        self.particle_positions = [[0.0, 0.0] for _ in range(10)]

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
        initial_state = [random.random() * 0.1 - 0.05, random.random() * 0.1 - 0.05]
        super().add_agent(name, definition, initial_state)

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

    def render(self):
        for i, name in enumerate(self.agents.keys()):
            self.viewer.particle_positions[i] = self.agents[name]["state"]
        self.viewer.render()


class ParticleAgent:
    def __init__(self):
        self.observation = None

    def set_observation(self, observation):
        self.observation = observation

    def _dist_to_act(self, dist):
        a = dist - 1.0
        if a < -1.0:
            a = 0.0
        elif a > 1.0:
            a = 0.0
        return a

    def compute_action(self):
        noise = [random.random() * 2.0 - 1.0, random.random() * 2.0 - 1.0]
        act = [self._dist_to_act(self.observation[0]), self._dist_to_act(self.observation[1])]
        return [
            -act[0] * 50.0 + noise[0],
            -act[1] * 50.0 + noise[1]
        ]


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
