from ghimopy.interfaces.environment_agent_interface import EnvironmentAgentInterface


class EnvironmentBlindAgentInterface(EnvironmentAgentInterface):
    def __init__(self, environment, agent):
        super().__init__(environment, agent)
        self.environment = environment
        self.agent = agent

    def act(self, action):
        self.environment.agents[self.agent.name]["action"] = action

    def observe(self):
        return None

