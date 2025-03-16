#!/usr/bin/env python3
import math
import random

from ghimopy.environments.unicycles_environment import UnicyclesEnvironment
from ghimopy.interfaces.environment_agent_interface import EnvironmentAgentInterface
from ghimopy.agents.agent import Agent
from ghimopy.viewers.unicycles_mpl_viewer import UnicyclesMplViewer
from ghimopy.geometry2d import angle_diff


def polar_coordinates_ctrl(rho, gamma, delta, k1=1.0, k2=3.0, k3=2.0):
    # see https://www.diag.uniroma1.it/~labrob/pub/papers/CST02.pdf
    # see https://www.researchgate.net/publication/225543929_Control_of_Wheeled_Mobile_Robots_An_Experimental_Overview
    # see https://www.researchgate.net/profile/Giuseppe-Oriolo-3/publication/3332395_WMR_control_via_dynamic_feedback_linearization_Design_implementation_and_experimental_validation/links/5480795f0cf2ccc7f8bd061f/WMR-control-via-dynamic-feedback-linearization-Design-implementation-and-experimental-validation.pdf
    v = k1 * rho * math.cos(gamma)
    w = k2 * gamma + k1 * (math.sin(gamma) * math.cos(gamma)) / gamma * (gamma + k3 * delta)
    return (v, w)


class UnicycleControlLawAgent(Agent):
    def step(self):
        rho, gamma, delta = self.interface.get_observation()
        v, w = polar_coordinates_ctrl(rho, gamma, delta)
        self.interface.set_action((v, w))


class UnicycleEnvironmentLocalInterface(EnvironmentAgentInterface):
    def __init__(self, environment, agent):
        self.environment = environment
        self.agent = agent

    def set_action(self, action):
        self.environment.agents[self.agent.name]["action"] = action

    def get_observation(self):
        goal = self.environment.agents[self.agent.name]["goal"]
        pose = self.environment.agents[self.agent.name]["state"]
        return (
            math.hypot(goal[0] - pose[0], goal[1] - pose[1]),
            angle_diff(math.atan2(goal[1] - pose[1], goal[0] - pose[0]), pose[2]),
            angle_diff(pose[2], goal[2])
        )


env = UnicyclesEnvironment()

agent = UnicycleControlLawAgent("unicycle")
env.add_agent(agent, initial_state=[0.0, 0.0, 0.0])
env.set_agent_goal("unicycle", [2.0, 3.0, 0.5])
UnicycleEnvironmentLocalInterface.link(env, agent)

viewer = UnicyclesMplViewer(10.0, 10.0)
env.set_viewer(viewer)

env.reset()

while not env.viewer.exit_requested:
    agent.step()
    env.step()

    goal = env.agents["unicycle"]["goal"]
    pose = env.agents["unicycle"]["state"]

    if math.hypot(goal[0] - pose[0], goal[1] - pose[1]) < 0.1 and math.fabs(angle_diff(goal[2], pose[2])) < 0.05:
        env.set_agent_goal("unicycle", [random.uniform(-4.0, 4.0), random.uniform(-4.0, 4.0), random.uniform(0.0, math.pi * 2.0)])
