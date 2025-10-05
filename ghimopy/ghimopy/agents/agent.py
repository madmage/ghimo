from abc import ABC, abstractmethod


class Agent(ABC):
    def __init__(self, name, definition=None):
        self.name = name
        self.definition = definition
        self.interface = None

    @abstractmethod
    def step(self):
        pass

    def set_interface(self, interface):
        self.interface = interface
