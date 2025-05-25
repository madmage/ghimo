#!/usr/bin/env python3
import math
import random

from ghimopy.environments.unicycles_environment import UnicyclesEnvironment
from ghimopy.interfaces.environment_agent_interface import EnvironmentAgentInterface
from ghimopy.agents.agent import Agent
from ghimopy.viewers.unicycles_mpl_viewer import UnicyclesMplViewer
from ghimopy.geometry2d import angle_diff
from ghimopy.control2d import polar_coordinates_ctrl


class UnicycleControlLawAgent(Agent):
    def step(self):
        rho, gamma, delta = self.interface.observe()
        v, w = polar_coordinates_ctrl(rho, gamma, delta, self.definition["k1"], self.definition["k2"], self.definition["k3"])
        self.interface.act((v, w))


class UnicycleEnvironmentLocalInterface(EnvironmentAgentInterface):
    def __init__(self, environment, agent):
        self.environment = environment
        self.agent = agent

    def act(self, action):
        self.environment.agents[self.agent.name]["action"] = action

    def observe(self):
        goal = self.environment.agents[self.agent.name]["goal"]
        pose = self.environment.agents[self.agent.name]["state"]
        return (
            math.hypot(goal[0] - pose[0], goal[1] - pose[1]),
            angle_diff(math.atan2(goal[1] - pose[1], goal[0] - pose[0]), pose[2]),
            angle_diff(pose[2], goal[2])
        )


env = UnicyclesEnvironment()

for i in range(10):
    agent_name = f"unicycle{i+1:02d}"
    agent = UnicycleControlLawAgent(agent_name, {
        "k1": 1.0 + random.uniform(-0.2, 0.2),
        "k2": 3.0 + random.uniform(-0.2, 0.2),
        "k3": 2.0 + random.uniform(-0.2, 0.2),
    })
    env.add_agent(agent, initial_state=[0.0, 0.0, 0.0])
    env.set_agent_goal(agent.name, [2.0, 3.0, 0.5])
    UnicycleEnvironmentLocalInterface.link(env, agent)

viewer = UnicyclesMplViewer(10.0, 10.0)
env.set_viewer(viewer)

env.reset()
while not env.viewer.exit_requested:
    for agent in env.get_agents():
        agent.step()

    env.step()

    all_arrived = True
    for i in range(10):
        agent_name = f"unicycle{i+1:02d}"
        goal = env.agents[agent_name]["goal"]
        pose = env.agents[agent_name]["state"]
        if not (math.hypot(goal[0] - pose[0], goal[1] - pose[1]) < 0.1 and math.fabs(angle_diff(goal[2], pose[2])) < 0.05):
            all_arrived = False
            break

    if all_arrived:
        init_pose = [random.uniform(-4.0, 4.0), random.uniform(-4.0, 4.0), random.uniform(0.0, math.pi * 2.0)]
        for i in range(10):
            agent_name = f"unicycle{i+1:02d}"
            env.set_agent_goal(agent_name, init_pose)
