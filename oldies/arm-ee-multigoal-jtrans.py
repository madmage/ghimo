#!/usr/bin/env python3
import time
import copy
import random
import math
from ghimo.environments import TwoLinksPlanarArm
from ghimo.geometry import dist2

env = TwoLinksPlanarArm()
env.robot.link3_len = 0
env.visualizer(True)
env.reset()


class PlanNode:
    def __init__(self, start_state, action, end_state):
        self.start_state = start_state
        self.action = action
        self.end_state = end_state


class NaiveGeometricPlanner:
    def __init__(self, robot_model):
        self.robot_model = robot_model
        self.init_state = [0, 0, 0]
        self.goal = None

    def set_init_state(self, state):
        self.init_state = state

    def set_goal(self, ee_pos):
        self.goal = ee_pos

    def random_ee_pos(self):
        return [
                random.uniform(-10, 10),
                random.uniform(-10, 10),
        ]

    def compute_dist(self, a1, a2):
        return (a1[0] - a2[0]) ** 2 + (a1[1] - a2[1]) ** 2

    def extend_random(self, q_from, target_ee_pos):
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

    def extend(self, q_from, target_ee_pos):
        robot = copy.deepcopy(self.robot_model)
        robot.set_state(q_from)
        e = [target_ee_pos[0] - robot.ee[0], target_ee_pos[1] - robot.ee[1]]
        # ctrl = k J^-1 e
        k = 0.01
        if robot.angle2 == 0.0:
            return self.extend_random(q_from, target_ee_pos)
        ctrl_a1 = (e[0] * k * math.cos(robot.angle1 + robot.angle2) / (robot.link1_len * math.sin(robot.angle2))
                   + e[1] * k * math.sin(robot.angle1 + robot.angle2) / (robot.link1_len * math.sin(robot.angle2)))
        l1, l2 = robot.link1_len, robot.link2_len
        a1, a2 = robot.angle1, robot.angle2
        e_x, e_y = e[0], e[1]
        ctrl_a2 = -e_x*k*(l1*math.cos(a1) + l2*math.cos(a1 + a2))/(l1*l2*math.sin(a2)) - e_y*k*(l1*math.sin(a1) + l2*math.sin(a1 + a2))/(l1*l2*math.sin(a2))
        robot.angle1 += ctrl_a1
        robot.angle2 += ctrl_a2
        robot._update()
        print(f"Error before: {e[0]}, {e[1]}, after: {target_ee_pos[0] - robot.ee[0]}, {target_ee_pos[1] - robot.ee[1]}")
        return [ctrl_a1, ctrl_a2, 0], [robot.angle1, robot.angle2, robot.angle3, robot.ee[0], robot.ee[1]]

    def plan(self):
        self.robot_model.set_state(self.init_state[0:3])
        should_finish = False
        p = [PlanNode(None, None, self.init_state[0:3])]
        while not should_finish:
            act, q_new = self.extend(p[-1].end_state, self.goal)
            p.append(PlanNode(p[-1].end_state, act, q_new))
            dist = self.compute_dist(q_new[3:5], self.goal)
            if dist < 0.001:
                return p


planner = NaiveGeometricPlanner(robot_model=copy.deepcopy(env.robot))
env.reset()
while True:
    goal = [random.uniform(-5.0, 5.0), random.uniform(-5.0, 5.0)]
    env.show_goal("ee", goal)

    planner.set_init_state([env.robot.angle1, env.robot.angle2, env.robot.angle3])
    planner.set_goal(goal)
    plan = planner.plan()

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
            break

    time.sleep(1)
