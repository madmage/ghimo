import copy

from ghimopy.agents.agent import Agent


class Environment:
    def __init__(self):
        self.agents = {}
        self.viewer = None

    def set_viewer(self, viewer) -> None:
        viewer.set_environment(self)
        self.viewer = viewer

    def step(self) -> None:
        if self.viewer:
            self.viewer.render()

    def reset(self) -> None:
        for agent in self.agents.values():
            agent["state"] = copy.deepcopy(agent["initial_state"])

    def add_agent(self, agent: Agent, initial_state=None) -> None:
        self.agents[agent.name] = {
            "agent": agent,
            "initial_state": copy.deepcopy(initial_state),
            "state": copy.deepcopy(initial_state),
            "action": None,
        }

    def get_agents(self):
        return [agent_dict["agent"] for agent_dict in self.agents.values()]
