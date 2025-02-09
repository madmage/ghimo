import random
from ghimo.agent import Agent


class GridEnvironmentRandomAgent(Agent):
    def step(self):
        obs = self.environment_interface.get_observation()
        act = self.compute_action(obs)
        self.environment_interface.set_action(act)

    def compute_action(self, obs):
        return (random.randint(-1, 1), random.randint(-1, 1))