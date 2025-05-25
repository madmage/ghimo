import random
from ghimopy.agents.agent import Agent


class GridEnvironmentRandomAgent(Agent):
    def step(self):
        obs = self.interface.observe()
        act = self._compute_action(obs)
        self.interface.act(act)

    def _compute_action(self, obs):
        return (random.randint(-1, 1), random.randint(-1, 1))
