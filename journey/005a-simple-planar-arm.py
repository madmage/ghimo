import math
import matplotlib as mpl

from ghimopy.environments.environment import Environment
from ghimopy.agents.agent import Agent
from ghimopy.viewers.mpl_viewer import MplViewer
from ghimopy.interfaces.environment_blind_agent_interface import EnvironmentBlindAgentInterface


class TwoLinksPlanarArmMplViewer(MplViewer):
    def render_objects(self) -> None:
        agent = self.environment.agents[self.environment.agent_name]
        link1_segment = agent["link1"]
        link2_segment = agent["link2"]

        self._ax.add_line(mpl.lines.Line2D([link1_segment[0], link1_segment[2]], [link1_segment[1], link1_segment[3]]))
        self._ax.add_line(mpl.lines.Line2D([link2_segment[0], link2_segment[2]], [link2_segment[1], link2_segment[3]]))


class TwoLinksPlanarArmEnvironment(Environment):
    def __init__(self):
        super().__init__()
        self.agent_name = None

    def _update_links(self):
        agent = self.agents[self.agent_name]
        agent["link1"] = [
            0.0,
            0.0,
            agent["agent"].parameters["link1_length"] * math.cos(agent["state"][0]),
            agent["agent"].parameters["link1_length"] * math.sin(agent["state"][0]),
        ]
        agent["link2"] = [
            agent["link1"][2],
            agent["link1"][3],
            agent["link1"][2] + agent["agent"].parameters["link2_length"] * math.cos(agent["state"][0] + agent["state"][1]),
            agent["link1"][3] + agent["agent"].parameters["link2_length"] * math.sin(agent["state"][0] + agent["state"][1]),
        ]

    def add_agent(self, agent, initial_state=None):
        super().add_agent(agent, initial_state)
        self.agent_name = agent.name
        self._update_links()

    def step(self):
        super().step()
        if self.agents[self.agent_name]["action"] is not None:
            self.agents[self.agent_name]["state"][0] += self.agents[self.agent_name]["action"][0]
            self.agents[self.agent_name]["state"][1] += self.agents[self.agent_name]["action"][1]
            self.agents[self.agent_name]["action"] = None
        self._update_links()


class TwoLinksArmPlanarAgent(Agent):
    def __init__(self, name, parameters={}):
        super().__init__(name)
        self.parameters = parameters

    def step(self):
        super().step()
        self.interface.act([0.1, 0.1])


env = TwoLinksPlanarArmEnvironment()

agent = TwoLinksArmPlanarAgent("arm", parameters={"link1_length": 2.0, "link2_length": 1.0})
env.add_agent(agent, initial_state=[0.0, 0.0])
EnvironmentBlindAgentInterface.link(env, agent)

viewer = TwoLinksPlanarArmMplViewer(10.0, 10.0)
env.set_viewer(viewer)

env.reset()
while not env.viewer.exit_requested:
    agent.step()
    env.step()
