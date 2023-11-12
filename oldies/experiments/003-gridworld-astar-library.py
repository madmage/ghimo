#!/usr/bin/env python3
import random
from ghirmo.environments.grid_world import GridWorld
from ghirmo.agents.astar_grid_agent import AStarGridAgent

random.seed(42)

ENV_WIDTH = 10
ENV_HEIGHT = 5
env = GridWorld(ENV_WIDTH, ENV_HEIGHT)
agent = AStarGridAgent(static_info={"env_width": ENV_WIDTH, "env_height": ENV_HEIGHT})
#env.add_agent("agent", [random.randint(0, ENV_WIDTH - 1), random.randint(0, ENV_HEIGHT - 1)])
#env.set_agent_goal("agent", (random.randint(0, ENV_WIDTH - 1), random.randint(0, ENV_HEIGHT - 1)))
env.add_agent("agent", [6, 1])
env.set_agent_goal("agent", [0, 1])
env.reset()

# FIXME there is a problem: the goal cell can be also an obstacle cell

for _ in range(10):
    env.render()
    print(f"Agent at {env.agents['agent']['state'][0]}, {env.agents['agent']['state'][1]}")
    obs = env.get_agent_observation("agent")
    act = agent.compute_action(obs)
    if act:
        print(f"Agent action: {act[0]}, {act[1]}")
    else:
        print("Cannot find a good action for the agent")
    env.set_agent_action("agent", act)
    env.step()
    feedback = env.get_feedback_for_agent("agent")
    agent.process_feedback(feedback)
    print("")
    if env.agent_has_finished("agent"):
        env.render()
        break

# OpenAI gym loop
# env.render()
# obs = env.reset()
# loop:
#   action = compute_action(obs)  # env.action_space.sample()
#   obs, reward, done, info = env.step(action)

# Ghirmo loop
# env.reset()
# loop:
#   obs = env.get_agent_observation(agent_name)
#   action = compute_action(obs)
#   env.set_agent_action(agent_name, action)
#   env.render()
#   env.step()
#   feedback = env.get_feedback_for_agent("agent")
#   agent.process_feedback(feedback)
#   if env.agent_has_finished(agent_name):
#       break

# check: agent may want to display some information
