class Agent:
    def __init__(self, name, definition=None):
        self.name = name
        self.definition = definition
        self.environment_interface = None

    def step(self):
        pass
