import copy
from .astar import AStar


class GridAStar(AStar):
    def __init__(self):
        super().__init__()
        self.info = {}

    def _goal_reached(self, state, goal):
        return state == goal

    def _generate_actions(self, state):
        actions = []
        for dx in [-1, 0, 1]:
            for dy in [-1, 0, 1]:
                action = [dx, dy]
                if state[0] + action[0] < 0 or state[0] + action[0] >= self.info["grid_width"]:
                    continue
                if state[1] + action[1] < 0 or state[1] + action[1] >= self.info["grid_height"]:
                    continue
                if self.info["cells"][state[0] + action[0]][state[1] + action[1]] == "*":
                    continue
                actions.append(action)
        return actions

    def _forward_action(self, state, action):
        new_state = copy.deepcopy(state)
        new_state[0] += action[0]
        new_state[1] += action[1]
        new_state[0] = max(new_state[0], 0)
        new_state[1] = max(new_state[1], 0)
        new_state[0] = min(new_state[0], self.info["grid_width"] - 1)
        new_state[1] = min(new_state[1], self.info["grid_height"] - 1)
        return new_state

