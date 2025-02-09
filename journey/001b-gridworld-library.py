#!/usr/bin/env python3
from ghimo.environments.grid_environment import GridEnvironment, GridEnvironmentNaiveInterface
from ghimo.grid_environment_random_agent import GridEnvironmentRandomAgent
from ghimo.viewers.grid_environment_console_viewer import GridEnvironmentConsoleViewer

ENV_WIDTH = 80
ENV_HEIGHT = 40
env = GridEnvironment()
env.random_definition(ENV_WIDTH, ENV_HEIGHT, obstacle_probability=0.1)

agent = GridEnvironmentRandomAgent("agent1")
env.add_agent(agent)

GridEnvironmentNaiveInterface.link(env, agent)

viewer = GridEnvironmentConsoleViewer(clear_screen=True, wait_time=0.1)
env.set_viewer(viewer)

env.reset()
for _ in range(100000):
    agent.step()
    env.step()

