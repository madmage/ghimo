from abc import ABC, abstractmethod
from ghimo.environments.environment import Environment
from ghimo.agents.agent import Agent

class EnvironmentAgentInterface(ABC):
    @classmethod
    def link(cls, environment: Environment, agent: Agent) -> None:
        interface = cls(environment, agent)
        interface.agent.interface = interface

    def __init__(self, environment: Environment, agent: Agent):
        self.environment = environment
        self.agent = agent

    @abstractmethod
    def set_action(self, action) -> None:
        pass

    @abstractmethod
    def get_observation(self):
        return None
