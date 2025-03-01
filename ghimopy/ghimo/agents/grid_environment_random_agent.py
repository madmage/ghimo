import random
from ghimo.agents.agent import Agent


class GridEnvironmentRandomAgent(Agent):
    def step(self):
        obs = self.interface.get_observation()
        act = self.compute_action(obs)
        self.interface.set_action(act)

    def compute_action(self, obs):
        return (random.randint(-1, 1), random.randint(-1, 1))