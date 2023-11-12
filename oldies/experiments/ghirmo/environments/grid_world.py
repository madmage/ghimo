import random
from termcolor import colored
from .environment_base import EnvironmentBase


class GridWorld(EnvironmentBase):
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.cells = []
        self.agents = {}    # each agent has "state", "feedback", "action" and "goal"
        for x in range(width):
            row = [1 if random.random() < 0.3 else 0 for y in range(height)]
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
                col = "grey" if self.cells[x][y] == 1 else "white"
                hl = "on_white" if self.cells[x][y] == 1 else None
                for agent_name, agent in self.agents.items():
                    if agent["state"][0] == x and agent["state"][1] == y:
                        print(colored("A", "cyan", hl, attrs=["bold"]), end="")
                        draw_cell=False
                        break
                    if agent["goal"][0] == x and agent["goal"][1] == y:
                        print(colored("G", "green", hl, attrs=["bold"]), end="")
                        draw_cell=False
                        break
                if draw_cell:
                    print(colored("·" if self.cells[x][y] == 0 else " ", col, hl), end="")
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
            if action is None:
                continue
            if action[0] == 1:
                self.agents[agent_name]["state"][0] = min(self.agents[agent_name]["state"][0] + 1, self.width - 1)
            elif action[0] == -1:
                self.agents[agent_name]["state"][0] = max(self.agents[agent_name]["state"][0] - 1, 0)
            if action[1] == 1:
                self.agents[agent_name]["state"][1] = min(self.agents[agent_name]["state"][1] + 1, self.height - 1)
            elif action[1] == -1:
                self.agents[agent_name]["state"][1] = max(self.agents[agent_name]["state"][1] - 1, 0)
            agent_obj["action"] = None

