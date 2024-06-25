from typing import Tuple, Any, Dict
from copy import deepcopy


class EnvironmentBase:
    def __init__(self):
        self.agents = {}

    def add_agent(self, name: str, definition: Dict = None, initial_state: Any = None) -> None:
        self.agents[name] = {
            "name": name,
            "definition": definition,
            "state": deepcopy(initial_state) if initial_state else None,
            "initial_state": deepcopy(initial_state) if initial_state else None,
            "action": None,
            "observation": None,
        }

    def reset(self) -> None:
        for name, agent in self.agents.items():
            agent["state"] = deepcopy(agent["initial_state"])

    def render(self) -> bool:
        return True

    def set_agent_action(self, agent_name: str, action: Any) -> None:
        self.agents[agent_name]["action"] = action

    def set_agent_goal(self, agent_name: str, goal):
        pass

    def get_agent_observation(self, agent_name: str) -> Any:
        return self.agents[agent_name]["observation"]

    def get_feedback_for_agent(self, agent_name: str) -> Tuple[float, bool]:
        pass

    def agent_has_finished(self, agent_name) -> bool:
        pass

    def step(self):
        """ Using the current actions of the agents, perform a simulation step
        and compute the observations they will get when they call get_agent_observation
        """
        pass
