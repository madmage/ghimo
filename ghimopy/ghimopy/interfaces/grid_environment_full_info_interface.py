from ghimopy.interfaces.environment_agent_interface import EnvironmentAgentInterface


class GridEnvironmentFullInfoInterface(EnvironmentAgentInterface):
    def observe(self):
        return {
            "map": self.environment.cells,
            "agent_position": self.environment.agents[self.agent.name]["state"],
            "goal": self.environment.agents[self.agent.name]["goal"]
        }

    def act(self, action):
        self.environment.agents[self.agent.name]["action"] = action

    def set_agent_info(self, agent_info):
        self.environment.agents[self.agent.name]["info"] = agent_info
