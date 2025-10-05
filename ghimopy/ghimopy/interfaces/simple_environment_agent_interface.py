from abc import ABC, abstractmethod
from ghimopy.environments.simple_environment import SimpleEnvironment
from ghimopy.agents.agent import Agent


class SimpleEnvironmentAgentInterface(ABC):
    @classmethod
    def link(cls, environment: SimpleEnvironment, agent: Agent) -> None:
        interface = cls(environment, agent)
        interface.agent.interface = interface

    def __init__(self, environment: SimpleEnvironment, agent: Agent):
        self.environment = environment
        self.agent = agent

    @abstractmethod
    def act(self, action) -> None:
        pass

    @abstractmethod
    def observe(self):
        return None