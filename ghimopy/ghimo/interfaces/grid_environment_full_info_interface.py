from ghimo.interfaces.environment_agent_interface import EnvironmentAgentInterface


class GridEnvironmentFullInfoInterface(EnvironmentAgentInterface):
    def get_observation(self):
        return {
            "map": self.environment.cells,
            "agent_position": self.environment.agents[self.agent.name]["state"],
            "goal": self.environment.agents[self.agent.name]["goal"]
        }

    def set_action(self, action):
        self.environment.agents[self.agent.name]["action"] = action

    def set_agent_info(self, agent_info):
        self.environment.agents[self.agent.name]["info"] = agent_info
