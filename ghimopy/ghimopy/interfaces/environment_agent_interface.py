from abc import ABC, abstractmethod
from ghimopy.environments.environment import Environment
from ghimopy.agents.agent import Agent


class EnvironmentAgentInterface(ABC):
    @classmethod
    def link(cls, environment: Environment, agent: Agent) -> None:
        interface = cls(environment, agent)
        interface.agent.interface = interface

    def __init__(self, environment: Environment, agent: Agent):
        self.environment = environment
        self.agent = agent

    @abstractmethod
    def act(self, action) -> None:
        pass

    @abstractmethod
    def observe(self):
        return None
