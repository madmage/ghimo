#!/usr/bin/env python3
from ghimopy.environments.grid_environment import GridEnvironment
from ghimopy.interfaces.grid_environment_full_info_interface import GridEnvironmentFullInfoInterface
from ghimopy.viewers.grid_environment_console_viewer import GridEnvironmentConsoleViewer
from ghimopy.agents.agent import Agent


class GridEnvironmentGreedyAgent(Agent):
    def step(self):
        obs = self.interface.observe()
        act = self._compute_action(obs)
        self.interface.act(act)

    def _compute_action(self, obs):
        agent_position = obs["agent_position"]
        agent_goal = obs["goal"]
        self.interface.set_agent_info({
            "goal": agent_goal,
        })
        act = [agent_goal[0] - agent_position[0], agent_goal[1] - agent_position[1]]
        if act[0]:
            act[0] /= abs(act[0])
        if act[1]:
            act[1] /= abs(act[1])
        return act


ENV_WIDTH = 80
ENV_HEIGHT = 40
env = GridEnvironment()
env.random_definition(ENV_WIDTH, ENV_HEIGHT, obstacle_probability=0.1)

agent = GridEnvironmentGreedyAgent("agent1")
env.add_agent(agent)
env.set_agent_goal("agent1", [int(ENV_WIDTH / 2), int(ENV_HEIGHT / 2)])
GridEnvironmentFullInfoInterface.link(env, agent)

viewer = GridEnvironmentConsoleViewer(clear_screen=False, wait_time=1.0)
env.set_viewer(viewer)

env.reset()
while True:
    agent.step()
    env.step()
