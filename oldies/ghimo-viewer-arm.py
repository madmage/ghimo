#!/usr/bin/env python3
import time
import copy
import random
import math
from ghimo.environments import TwoLinksPlanarArm
from ghimo.geometry import dist2

env = TwoLinksPlanarArm()
env.visualizer(True)
env.reset()


class Rrt:
    class Node:
        def __init__(self):
            self.state = None
            self.parent = None
            self.children = []

        def __str__(self):
            return f"Node with state: {self.state}"

    def __init__(self):
        self.root = None

    def add_node(self, state, parent):
        new_node = Rrt.Node()
        new_node.state = copy.deepcopy(state)
        new_node.parent = parent
        if parent:
            parent.children.append(new_node)
        else:
            self.root = new_node
        return new_node

    def find_closest_node(self, state):
        cdist = 100000
        cnode = None
        visit = [self.root]
        while len(visit):
            n = visit.pop()
            visit.extend(n.children)
            dist = dist2(n.state, state)
            if dist < cdist:
                cdist = dist
                cnode = n
        return cnode


class RrtPlanner:
    def __init__(self):
        self.init_state = [0, 0, 0]
        self.goal_state = [1.0, 0.5, 0.2]

    def set_init_state(self, state):
        self.init_state = state

    def set_goal_state(self, state):
        self.goal_state = state

    def compute_plan(self, rrt, node):
        p = [node]
        while True:
            if p[-1].parent is None:
                break
            p.append(p[-1].parent)
        p.reverse()
        return p

    def random_state(self):
        return [
                random.uniform(-10, 10),
                random.uniform(-10, 10),
                random.uniform(-10, 10),
        ]

    def extend(self, q_from, q_to):
        q_new = copy.deepcopy(q_from)
        ln = math.sqrt(sum([(q_to[i] - q_from[i]) ** 2 for i in range(len(q_new))]))
        for i in range(len(q_new)):
            q_new[i] += (q_to[i] - q_from[i]) / ln * 0.01
        return q_new

    def plan(self):
        rrt = Rrt()
        rrt.add_node(self.init_state, parent=None)
        should_finish = False
        while not should_finish:
            if random.random() < 0.8:
                q_rand = self.random_state()
            else:
                q_rand = self.goal_state
            node_near = rrt.find_closest_node(q_rand)
            q_new = self.extend(node_near.state, q_rand)
            node_new = rrt.add_node(q_new, parent=node_near)
            dist = max([math.fabs(x - y) for x, y in zip(q_new, self.goal_state)])
            if dist < 0.01:
                p = self.compute_plan(rrt, node_new)
                return p


env.set_goal([1.3, -2.3, 1.45])

rrtplanner = RrtPlanner()
rrtplanner.set_init_state([0, 0, 0])
rrtplanner.set_goal_state([1.3, -2.3, 1.45])
plan = rrtplanner.plan()
for i in range(len(plan)):
    # performs a naive configuration -> action conversion
    if i < len(plan) - 1:
        for c in range(3):
            plan[i].state[c] = plan[i+1].state[c] - plan[i].state[c]
del plan[-1]

print("PLAN:")
state = [0, 0, 0]
for p in plan:
    print(p.state, state)
    state[0] += p.state[0]
    state[1] += p.state[1]
    state[2] += p.state[2]

plan_idx = 0

while True:
    observation = env.sense()
    env.act(plan[plan_idx].state)
    env.render()
    time.sleep(0.001)
    plan_idx += 1
    if plan_idx >= len(plan):
        env.reset()
        plan_idx = 0
