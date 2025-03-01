from ghimo.interfaces.environment_agent_interface import EnvironmentAgentInterface


class EnvironmentBlindAgentInterface(EnvironmentAgentInterface):
    def __init__(self, environment, agent):
        super().__init__(environment, agent)
        self.environment = environment
        self.agent = agent

    def set_action(self, action):
        self.environment.agents[self.agent.name]["action"] = action

    def get_observation(self):
        return None

