#!/usr/bin/env python3
import random
import copy


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


class PriorityQueue:
    def __init__(self):
        self.queue = []

    def push(self, item, value):
        self.queue.append([item, value])
        self.queue.sort(key=lambda x: x[1])

    def pop(self):
        return self.queue.pop(0)

    def get_value(self, item):
        return [v[1] for v in self.queue if v[0] == item][0]

    def update_value(self, item, value):
        for s in self.queue:
            if s[0] == item:
                s[1] = value
                break
        self.queue.sort(key=lambda x: x[1])

    def __contains__(self, item):
        return item in (x[0] for x in self.queue)

    def empty(self):
        return len(self.queue) == 0


class AStar:
    def __init__(self):
        self.info = {}

    def _extract_plan(self, to_reach, cur_state):
        plan = []
        plan.append((cur_state, None))  # goal, no action afterwards
        while cur_state:
            for ss in to_reach:
                if ss[2] == cur_state:
                    if ss[0]:
                        plan.append((ss[0], ss[1]))  # state, action
                    cur_state = ss[0]
                    break
        plan.reverse()
        return plan

    def _goal_reached(self, state, goal):
        pass

    def _generate_actions(self, state):
        pass

    def _forward_action(self, state, action):
        pass

    def compute_plan(self, state, goal):
        open_pq = PriorityQueue()
        open_pq.push(state, 0)
        visited = []
        to_reach = [(None, None, state)]
        for _ in range(1000):
            if open_pq.empty():
                # failed
                return []
            cur_state, cur_cost = open_pq.pop()
            visited.append(cur_state)
            if self._goal_reached(cur_state, goal):
                return self._extract_plan(to_reach, cur_state)

            actions = self._generate_actions(cur_state)
            for action in actions:
                new_state = self._forward_action(cur_state, action)
                new_cost = cur_cost + 1
                if new_state in visited:
                    continue
                elif new_state in open_pq:
                    old_cost = open_pq.get_value(new_state)
                    if new_cost < old_cost:
                        open_pq.update(new_state, new_cost)
                        idx_in_to_reach = [i for i, v in enumerate(to_reach) if v[2] == new_state][0]
                        to_reach[idx_in_to_reach] = (cur_state, action, new_state)
                else:
                    open_pq.push(new_state, new_cost)
                    to_reach.append((cur_state, action, new_state))


class GridAStar(AStar):
    def __init__(self):
        super().__init__()
        self.info = {}

    def _goal_reached(self, state, goal):
        return state == goal

    def _generate_actions(self, state):
        actions = []
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                action = [dx, dy]
                if state[0] + action[0] < 0 or state[0] + action[0] >= self.info["grid_width"]:
                    continue
                if state[1] + action[1] < 0 or state[1] + action[1] >= self.info["grid_height"]:
                    continue
                if self.info["cells"][state[0] + action[0]][state[1] + action[1]] == "*":
                    continue
                actions.append(action)
        return actions

    def _forward_action(self, state, action):
        new_state = copy.deepcopy(state)
        new_state[0] += action[0]
        new_state[1] += action[1]
        new_state[0] = max(new_state[0], 0)
        new_state[1] = max(new_state[1], 0)
        new_state[0] = min(new_state[0], self.info["grid_width"] - 1)
        new_state[1] = min(new_state[1], self.info["grid_height"] - 1)
        return new_state


class AStarAgent:
    def __init__(self, static_info):
        self.env_width = static_info["env_width"]
        self.env_height = static_info["env_height"]
        self.planner = GridAStar()
        self.planner.info["grid_width"] = static_info["env_width"]
        self.planner.info["grid_height"] = static_info["env_height"]

    def compute_action(self, obs):
        self.planner.info["cells"] = obs["cells"]
        plan = self.planner.compute_plan(obs["state"], obs["goal"])
        return plan[0][1]

    def process_feedback(self, feedback):
        pass


ENV_WIDTH = 10
ENV_HEIGHT = 5
env = GridEnvironment(ENV_WIDTH, ENV_HEIGHT)
agent = AStarAgent(static_info={"env_width": ENV_WIDTH, "env_height": ENV_HEIGHT})
#env.add_agent("agent", [random.randint(0, ENV_WIDTH - 1), random.randint(0, ENV_HEIGHT - 1)])
#env.set_agent_goal("agent", (random.randint(0, ENV_WIDTH - 1), random.randint(0, ENV_HEIGHT - 1)))
env.add_agent("agent", [6, 1])
env.set_agent_goal("agent", [0, 1])
env.reset()


for _ in range(10):
    env.render()
    print(f"Agent at {env.agents['agent']['state'][0]}, {env.agents['agent']['state'][1]}")
    obs = env.get_agent_observation("agent")
    act = agent.compute_action(obs)
    print(f"Agent action: {act[0]}, {act[1]}")
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
#   env.step()
