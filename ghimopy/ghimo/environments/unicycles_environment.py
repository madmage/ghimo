#!/usr/bin/env python3
import math

from ghimo.environments.environment import Environment


class UnicyclesEnvironment(Environment):
    def set_agent_goal(self, agent_name, goal):
        self.agents[agent_name]["goal"] = goal

    def step(self):
        super().step()
        dt = 0.1
        for agent in self.agents.values():
            if "action" in agent:
                x0, y0, theta0 = agent["state"]
                v0, w0 = agent["action"]
                x1 = x0 + math.cos(theta0) * v0 * dt
                y1 = y0 + math.sin(theta0) * v0 * dt
                theta1 = theta0 + w0 * dt
                agent["state"] = x1, y1, theta1
