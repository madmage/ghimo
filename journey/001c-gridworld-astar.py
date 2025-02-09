#!/usr/bin/env python3
import sys
import random
from ghimo.environments.grid_environment import GridEnvironment, GridEnvironmentNaiveInterface
from ghimo.viewers.grid_environment_console_viewer import GridEnvironmentConsoleViewer
from ghimo.agent import Agent


class GridEnvironmentGreedyAgent(Agent):
    def __init__(self, name, definition=None):
        self.goal = None
        super().__init__(name, definition)

    def set_goal(self, goal):
        self.goal = goal

    def step(self):
        obs = self.environment_interface.get_observation()
        act = self._compute_action(obs)
        self.environment_interface.set_action(act)
        self.environment_interface.set_agent_info({
            "goal": self.goal,
        })

    def _compute_action(self, obs):
        agent_position = obs["agent_position"]
        agent_goal = self.goal
        act = [agent_goal[0] - agent_position[0], agent_goal[1] - agent_position[1]]
        if act[0]:
            act[0] /= abs(act[0])
        if act[1]:
            act[1] /= abs(act[1])
        return act


class GridEnvironmentAStarAgent(Agent):
    def __init__(self, name, definition=None):
        self.goal = None
        self.planner = AStarPlanner()
        super().__init__(name, definition)

    def set_goal(self, goal):
        self.goal = goal

    def step(self):
        obs = self.environment_interface.get_observation()
        self.planner.set_map(obs["map"])
        path = self.planner.plan(obs["agent_position"], self.goal)
        act = self._compute_action(obs, path)
        self.environment_interface.set_action(act)
        self.environment_interface.set_agent_info({
            "path": path,
            "goal": self.goal,
        })

    def _compute_action(self, obs, path):
        print(path)
        act = path[0][1]
        return act


class PriorityQueue:
    def __init__(self):
        self.queue = []

    def pop(self):
        return self.queue.pop(0)

    def push(self, element, value):
        self.queue.append((element, value))
        self.queue.sort(key=lambda x: x[1])
        #print("Queue(1):", self.queue)

    def update(self, element, value):
        idx = next((i for i, v in enumerate(self.queue) if v[0] == element), None)
        #print("Q:", element, self.queue[idx][1], value)
        if idx is None:
            self.push(element, value)
        else:
            self.queue[idx] = (element, value)
        #print("Queue(2a):", self.queue)
        self.queue.sort(key=lambda x: x[1])
        #print("Queue(2b):", self.queue)

    def clear(self):
        self.queue.clear()

    def empty(self):
        return len(self.queue) == 0

    def get_cost(self, element):
        return next((cost for elem, cost in self.queue if elem == element), None)


class AStarPlanner:
    def __init__(self):
        pass

    def build_path(self, predecessors, last_state):
        path = []
        s = predecessors[tuple(last_state)]
        while s:
            path.append(s)
            s = predecessors[tuple(s[0])]
        path.reverse()
        return path

    def set_map(self, m):
        self.map = m

    def expand(self, state, cost):
        possible_actions = [
            [1, 1], [1, 0], [1, -1],
            [0, 1], [0, -1],
            [-1, 1], [-1, 0], [-1, -1]]

        ret = []
        for act in possible_actions:
            nstate = [state[0] + act[0], state[1] + act[1]]
            if nstate[0] < 0 or nstate[1] < 0 or nstate[1] >= len(self.map[0]) or nstate[0] >= len(self.map):
                continue
            #print(nstate[0], nstate[1], len(self.map[0]), len(self.map))
            if self.map[nstate[0]][nstate[1]] == "X":
                continue

            act_cost = abs(act[0]) + abs(act[1])
            if act_cost == 2:
                act_cost = 1.6
            ret.append([act, nstate, cost + act_cost])
        #print(f"Starting from state {state}, successors:")
        #for s in ret:
        #    print(f"  {s}")
        return ret

    def is_goal(self, state, goal):
        return state == goal

    def plan(self, initial_state, goal):
        frontier = PriorityQueue()
        frontier.push(initial_state, 0)
        closed = []
        predecessors = {tuple(initial_state): None}
        # predecessors contains a map from state to
        # tuples (starting state, action, ending state, cost)
        while not frontier.empty():
            state, gcost = frontier.pop()
            if self.is_goal(state, goal):
                path = self.build_path(predecessors, state)
                #print(path)
                return path
            closed.append(state)

            successors = self.expand(state, gcost)
            for action, nstate, ngcost in successors:
                if nstate in closed:
                    continue

                add_it = False
                old_cost = frontier.get_cost(nstate)
                if old_cost is None:
                    frontier.push(nstate, ngcost)
                    add_it = True
                elif gcost < old_cost:
                    frontier.update(nstate, ngcost)
                    add_it = True
                if add_it:
                    predecessors[tuple(nstate)] = (state, action, nstate, ngcost)

        return False

def check_path(path, check):
    for x, y in zip(path, check):
        if x[0] != y:
            return False
    return path[-1][2] == check[-1]


p = AStarPlanner()
p.set_map(["....", "....", "....", "...."])

tests = [
    {"init": [1, 1], "goal": [3, 3], "path": [[1, 1], [2, 2], [3, 3]]},
    {"init": [1, 1], "goal": [3, 1], "path": [[1, 1], [2, 1], [3, 1]]},
    {"init": [3, 1], "goal": [1, 1], "path": [[3, 1], [2, 1], [1, 1]]},
    {"init": [1, 1], "goal": [3, 3], "path": [[1, 1], [2, 2], [3, 3]]},
    ]

for test in tests:
    path = p.plan(test["init"], test["goal"])
    print(test["init"], test["goal"], path, check_path(path, test["path"]))

#sys.exit(0)

ENV_WIDTH = 80
ENV_HEIGHT = 40
env = GridEnvironment()
env.random_definition(ENV_WIDTH, ENV_HEIGHT, obstacle_probability=0.1)

agent = GridEnvironmentAStarAgent("agent1")
env.add_agent(agent)

GridEnvironmentNaiveInterface.link(env, agent)

viewer = GridEnvironmentConsoleViewer(clear_screen=False, wait_time=1.0)
env.set_viewer(viewer)

agent.set_goal([int(ENV_WIDTH / 2), int(ENV_HEIGHT / 2)])

env.reset()
for _ in range(100000):
    agent.step()
    env.step()
