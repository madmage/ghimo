#!/usr/bin/env python3
from ghimopy.environments.grid_environment import GridEnvironment
from ghimopy.interfaces.environment_blind_agent_interface import EnvironmentBlindAgentInterface
from ghimopy.agents.grid_environment_random_agent import GridEnvironmentRandomAgent
from ghimopy.viewers.grid_environment_console_viewer import GridEnvironmentConsoleViewer

ENV_WIDTH = 80
ENV_HEIGHT = 40
env = GridEnvironment()
env.random_definition(ENV_WIDTH, ENV_HEIGHT, obstacle_probability=0.1)

agent = GridEnvironmentRandomAgent("agent1")
env.add_agent(agent)

agent.set_interface(EnvironmentBlindAgentInterface(env, agent))

viewer = GridEnvironmentConsoleViewer(clear_screen=True, wait_time=0.1)
env.set_viewer(viewer)

env.reset()
while True:
    agent.step()
    env.render()
    env.step()
