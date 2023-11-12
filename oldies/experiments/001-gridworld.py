#!/usr/bin/env python3
import random


class GridEnvironment:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.cells = []
        self.agents = {}    # each agent has "state", "feedback", "action" and "goal"
        for x in range(width):
            row = ["*" if random.random() < 0.3 else "." for y in range(height)]
            self.cells.append(row)
        self._current_feedback = [0.0, False]

    def add_agent(self, name, state):
        self.agents[name] = {"state": state, "feedback": None, "action": None, "goal": None}

    def reset(self):
        pass

    def render(self):
        for y in range(0, self.height):
            for x in range(0, self.width):
                draw_cell = True
                for agent_name, agent in self.agents.items():
                    if agent["state"][0] == x and agent["state"][1] == y:
                        print("A", end="")
                        draw_cell=False
                        break
                    if agent["goal"][0] == x and agent["goal"][1] == y:
                        print("G", end="")
                        draw_cell=False
                        break
                if draw_cell:
                    print("." if self.cells[x][y] == "." else "*", end="")
            print("")

    def set_agent_action(self, agent_name, action):
        self.agents[agent_name]["action"] = action

    def set_agent_goal(self, agent_name, goal):
        self.agents[agent_name]["goal"] = list(goal)

    def get_agent_observation(self, agent_name):
        agent = self.agents[agent_name]
        return {"state": agent["state"], "goal": agent["goal"], "cells": self.cells}

    def get_feedback_for_agent(self, agent_name):
        if self.agents[agent_name]["state"] == self.agents[agent_name]["goal"]:
            return (100.0, True)
        else:
            return (0.0, False)

    def agent_has_finished(self, agent_name):
        return self.get_feedback_for_agent(agent_name)[1]

    def step(self):
        for agent_name, agent_obj in self.agents.items():
            action = agent_obj["action"]
            if action[0] == 1:
                self.agents[agent_name]["state"][0] = min(self.agents[agent_name]["state"][0] + 1, self.width - 1)
            elif action[0] == -1:
                self.agents[agent_name]["state"][0] = max(self.agents[agent_name]["state"][0] - 1, 0)
            if action[1] == 1:
                self.agents[agent_name]["state"][1] = min(self.agents[agent_name]["state"][1] + 1, self.height - 1)
            elif action[1] == -1:
                self.agents[agent_name]["state"][1] = max(self.agents[agent_name]["state"][1] - 1, 0)
            agent_obj["action"] = None


class RandomAgent:
    def __init__(self, static_info):
        self.env_width = static_info["env_width"]
        self.env_height = static_info["env_height"]

    def compute_action(self, obs):
        return (random.randint(-1, 1), random.randint(-1, 1))

    def process_feedback(self, feedback):
        pass


class GreedyAgent:
    def __init__(self, static_info):
        self.env_width = static_info["env_width"]
        self.env_height = static_info["env_height"]

    def compute_action(self, obs):
        state = obs["state"]
        goal = obs["goal"]
        action = [0, 0]
        for s in [0, 1]:
            if state[s] > goal[s]:
                action[s] = -1
            elif state[s] < goal[s]:
                action[s] = 1
        return action

    def process_feedback(self, feedback):
        pass


ENV_WIDTH = 10
ENV_HEIGHT = 5
env = GridEnvironment(ENV_WIDTH, ENV_HEIGHT)
# agent = RandomAgent(static_info={"env_width": ENV_WIDTH, "env_height": ENV_HEIGHT})
agent = GreedyAgent(static_info={"env_width": ENV_WIDTH, "env_height": ENV_HEIGHT})
env.add_agent("agent", [random.randint(0, ENV_WIDTH - 1), random.randint(0, ENV_HEIGHT - 1)])
env.set_agent_goal("agent", (random.randint(0, ENV_WIDTH - 1), random.randint(0, ENV_HEIGHT - 1)))
env.reset()


for _ in range(100000):
    # OpenAI Gym env.render()
    env.render()
    print(f"Agent at {env.agents['agent']['state'][0]}, {env.agents['agent']['state'][1]}")
    # Gym: action = env.action_space.sample()
    obs = env.get_agent_observation("agent")
    # Gym: obs, reward, done, info = env.step(action)
    act = agent.compute_action(obs)
    print(f"Agent action: {act[0]}, {act[1]}")
    env.set_agent_action("agent", act)
    env.step()
    feedback = env.get_feedback_for_agent("agent")
    agent.process_feedback(feedback)
    if env.agent_has_finished("agent"):
        break
    print("")

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
#   env.step()
