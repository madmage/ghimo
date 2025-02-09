import copy

from ghimo.agent import Agent


class Environment:
    def __init__(self):
        self.agents = {}
        self.viewer = None

    def set_viewer(self, viewer) -> None:
        viewer.set_environment(self)
        self.viewer = viewer

    def step(self) -> None:
        # the derived classes should always call the super().step()
        # *before* changing the state, so that it can show
        # the state and the action that the agent wants to perform
        # in that state, *before* the state changes
        if self.viewer:
            self.viewer.render()

    def reset(self) -> None:
        pass

    def add_agent(self, agent: Agent, initial_state) -> None:
        self.agents[agent.name] = {
            "initial_state": copy.deepcopy(initial_state),
            "state": copy.deepcopy(initial_state),
            "interface": None,
            "action": None,
        }


class EnvironmentInterface:
    @classmethod
    def link(cls, environment: Environment, agent: Agent) -> None:
        interface = cls(environment, agent)
        interface.environment.agents[agent.name]["interface"] = interface
        interface.agent.environment_interface = interface

    def __init__(self, environment: Environment, agent: Agent):
        self.environment = environment
        self.agent = agent

    def set_agent(self, agent) -> None:
        self.agent = agent

    def set_environment(self, environment) -> None:
        self.environment = environment
