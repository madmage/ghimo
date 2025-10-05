import random
import copy
import time

from ghimopy.environments.simple_environment import SimpleEnvironment
from ghimopy.viewers.viewer import Viewer


class StreetEnvironment(SimpleEnvironment):
    def __init__(self):
        super().__init__()
        self.home = 7

    def add_agent(self, agent):
        super().add_agent(agent)
        self.agents[agent.name] = {
            "state": [random.randint(0, 10)],
            "action": None,
        }

    def step(self):
        super().step()
        for agent_name, agent_dict in self.agents.items():
            action = agent_dict["action"]
            new_state = copy.deepcopy(self.agents[agent_name]["state"])
            if action[0] == "forward":
                new_state[0] = min(new_state[0] + 1, 9)
            elif action[0] == "backward":
                new_state[0] = max(0, new_state[0] - 1)
            agent_dict["action"] = None
            agent_dict["state"] = new_state


class StreetEnvironmentConsoleViewer(Viewer):
    def __init__(self, wait_time=1.0):
        self.wait_time = wait_time

    def render(self):
        for agent_name, agent in self.environment.agents.items():
            print(f"Agent {agent_name} is in place {agent['state'][0]}, home is in place {self.environment.home}, agent chooses '{agent['action'][0]}' as action")
        print("")
        time.sleep(self.wait_time)