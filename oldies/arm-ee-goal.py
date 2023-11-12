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
            self.start_state = None     # here state is [qa1, qa2, qa3, ee1, ee2]
            self.action = None          # here action is [da1, da2, da3]
            self.end_state = None
            self.parent = None
            self.children = []

        def __str__(self):
            return f"Node with start_state: {self.start_state}, action: {self.action}, end_state: {self.end_state}"

    def __init__(self):
        self.root = None

    def add_node(self, start_state, action, end_state, parent):
        new_node = Rrt.Node()
        new_node.start_state = copy.deepcopy(start_state)
        new_node.action = copy.deepcopy(action)
        new_node.end_state = copy.deepcopy(end_state)
        new_node.parent = parent
        if parent:
            parent.children.append(new_node)
        else:
            self.root = new_node
        return new_node

    def find_closest_node(self, ee_pos):
        cdist = 100000
        cnode = None
        visit = [self.root]
        while len(visit):
            n = visit.pop()
            visit.extend(n.children)
            dist = dist2(n.end_state[3:5], ee_pos)
            if dist < cdist:
                cdist = dist
                cnode = n
        return cnode


class RrtPlanner:
    def __init__(self, robot_model):
        self.robot_model = robot_model
        self.init_state = [0, 0, 0]
        self.goal = None

    def set_init_state(self, state):
        self.init_state = state

    def set_goal(self, ee_pos):
        self.goal = ee_pos

    def compute_plan(self, rrt, node):
        p = [node]
        while True:
            if p[-1].parent is None:
                break
            p.append(p[-1].parent)
        p.reverse()
        return p

    def random_ee_pos(self):
        return [
                random.uniform(-10, 10),
                random.uniform(-10, 10),
        ]

    def compute_dist(self, a1, a2):
        return (a1[0] - a2[0]) ** 2 + (a1[1] - a2[1]) ** 2

    def extend(self, q_from, target_ee_pos):
        robot = copy.deepcopy(self.robot_model)
        best_dist = 100000000
        for i in range(4):
            # compute a random action
            max_act = 0.2
            random_action = [random.uniform(-max_act, max_act), random.uniform(-max_act, max_act), random.uniform(-max_act, max_act)]

            # use the robot model to see the effect of the action
            robot.angle1, robot.angle2, robot.angle3 = q_from[0:3]
            robot.angle1 += random_action[0]
            robot.angle2 += random_action[1]
            robot.angle3 += random_action[2]
            robot._update()
            ee_pos = [robot.link3[2], robot.link3[3]]

            dist = self.compute_dist([ee_pos[0], ee_pos[1]], target_ee_pos)
            if dist < best_dist:
                best_dist = dist
                best_action = random_action
                best_q = [robot.angle1, robot.angle2, robot.angle3, robot.link3[2], robot.link3[3]]
        return best_action, best_q

    def plan(self):
        self.robot_model.angle1 = self.init_state[0]
        self.robot_model.angle2 = self.init_state[1]
        self.robot_model.angle3 = self.init_state[2]
        self.robot_model._update()
        best_dist = 10000
        rrt = Rrt()
        rrt.add_node(None, None, self.init_state + [self.robot_model.link3[2], self.robot_model.link3[3]], parent=None)
        should_finish = False
        while not should_finish:
            if random.random() < 1.0: #0.8:
                ee_rand = self.goal
            else:
                ee_rand = self.random_ee_pos()
            node_near = rrt.find_closest_node(ee_pos=ee_rand)
            action, q_new = self.extend(node_near.end_state, ee_rand)
            node_new = rrt.add_node(node_near.end_state, action, q_new, parent=node_near)
            dist = self.compute_dist(q_new[3:5], self.goal)
            if dist < best_dist:
                best_dist = dist
                print(f"Best dist: {best_dist}")
                if dist < 0.01:
                    p = self.compute_plan(rrt, node_new)
                    return p


env.show_goal("ee", [6.0, 4.0])

rrtplanner = RrtPlanner(robot_model=copy.deepcopy(env.robot))
rrtplanner.set_init_state([0, 0, 0])
rrtplanner.set_goal([6.0, 4.0])
plan = rrtplanner.plan()
for i in range(len(plan)):
    print(plan[i])

print("PLAN:")
state = [0, 0, 0]
for p in plan:
    if p.action is None:
        print(p.end_state)
    else:
        print(p.action, p.end_state)

plan_idx = 0

while True:
    observation = env.sense()
    env.act(plan[plan_idx].action)
    env.render()
    time.sleep(0.001)
    plan_idx += 1
    if plan_idx >= len(plan):
        env.reset()
        plan_idx = 0
