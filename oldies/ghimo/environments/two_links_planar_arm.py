import copy
import math
from ghimo.geometry import Line
from ghimo.visualizers import MatplotlibVisualizer


class Environment:
    def __init__(self):
        pass


class TwoLinksPlanarArm(Environment):
    class Robot(object):
        def __init__(self):
            self.link1_len = 5.0
            self.link2_len = 2.5
            self.link3_len = 2.5
            self.link1 = [0, 0, 0, 0]
            self.link2 = [0, 0, 0, 0]
            self.link3 = [0, 0, 0, 0]
            self.angle1 = 0
            self.angle2 = 0
            self.angle3 = 0
            self._update()

        def set_state(self, state):
            self.angle1 = state[0]
            self.angle2 = state[1]
            self.angle3 = state[2]
            self._update()

        def _update(self):
            self.link1 = [
                0, 0,
                self.link1_len * math.cos(self.angle1),
                self.link1_len * math.sin(self.angle1)]
            self.link2 = [
                self.link1[2], self.link1[3],
                self.link1[2] + self.link2_len * math.cos(self.angle1 + self.angle2),
                self.link1[3] + self.link2_len * math.sin(self.angle1 + self.angle2)]
            self.link3 = [
                self.link2[2], self.link2[3],
                self.link2[2] + self.link3_len * math.cos(self.angle1 + self.angle2 + self.angle3),
                self.link2[3] + self.link3_len * math.sin(self.angle1 + self.angle2 + self.angle3)]

            self.ee = [self.link3[2], self.link3[3]]

            #a1, a2, a3 = self.angle1, self.angle2, self.angle3
            #l1, l2, l3 = self.link1_len, self.link2_len, self.link3_len

            #link1[2] = l1 * math.cos(a1)
            #link1[3] = l1 * math.sin(a1)

            #link2[2] = link1[2] + l2 * math.cos(a1 + a2)
            #link2[3] = link1[3] + l2 * math.sin(a1 + a2)

            #self.link3[0] = link2[2] + l3 * math.cos(a1 + a2 + a3)
            #self.link3[1] = link2[3] + l3 * math.sin(a1 + a2 + a3)


            #self.link3[0] = link1[2] + l2 * math.cos(a1 + a2) + l3 * math.cos(a1 + a2 + a3)
            #self.link3[1] = link1[2] + l2 * math.cos(a1 + a2) + l3 * math.sin(a1 + a2 + a3)

    def __init__(self):
        self.robot = TwoLinksPlanarArm.Robot()
        self.robot.link1_len = 5.0
        self.robot.link2_len = 2.5
        self.robot.link3_len = 2.5
        self.goal = copy.deepcopy(self.robot)
        self.vis = None
        self.reset()

    def show_goal(self, goal_type, goal_desc):
        sz = 0.2
        self.vis.add_object(Line({"id": "goal.1", "c": "green"}))
        self.vis.update_object("goal.1", {"x1": goal_desc[0] - sz, "y1": goal_desc[1] - sz, "x2": goal_desc[0] + sz, "y2": goal_desc[1] + sz})
        self.vis.add_object(Line({"id": "goal.2", "c": "green"}))
        self.vis.update_object("goal.2", {"x1": goal_desc[0] - sz, "y1": goal_desc[1] + sz, "x2": goal_desc[0] + sz, "y2": goal_desc[1] - sz})

    def set_goal(self, goal):
        self.goal.angle1 = goal[0]
        self.goal.angle2 = goal[1]
        self.goal.angle3 = goal[2]
        self.goal._update()
        self.vis.update_object("goal.link1", dict(zip(["x1", "y1", "x2", "y2"], self.goal.link1)))
        self.vis.update_object("goal.link2", dict(zip(["x1", "y1", "x2", "y2"], self.goal.link2)))
        self.vis.update_object("goal.link3", dict(zip(["x1", "y1", "x2", "y2"], self.goal.link3)))

    def visualizer(self, on):
        if on:
            self.vis = MatplotlibVisualizer()
            self.vis.add_object(Line({"id": "robot.link1", "lw": 2.0}))
            self.vis.add_object(Line({"id": "robot.link2", "lw": 2.0}))
            self.vis.add_object(Line({"id": "robot.link3", "lw": 2.0}))
            self.vis.add_object(Line({"id": "goal.link1", "lw": 1.0, "c": "red"}))
            self.vis.add_object(Line({"id": "goal.link2", "lw": 1.0, "c": "red"}))
            self.vis.add_object(Line({"id": "goal.link3", "lw": 1.0, "c": "red"}))

    def render(self):
        if self.vis:
            keys = ["x1", "y1", "x2", "y2"]
            self.vis.update_object("robot.link1", dict(zip(keys, self.robot.link1)))
            self.vis.update_object("robot.link2", dict(zip(keys, self.robot.link2)))
            self.vis.update_object("robot.link3", dict(zip(keys, self.robot.link3)))
            self.vis.render()

    def reset(self):
        self.robot.angle1 = 0.0
        self.robot.angle2 = 0.0
        self.robot.angle3 = 0.0
        self.robot._update()

    def sense(self):
        return {
            "angle1": self.robot.angle1,
            "angle2": self.robot.angle2,
            "angle3": self.robot.angle3,
            "ee": (self.robot.link3[2], self.robot.link3[3]),
        }

    def act(self, action):
        if isinstance(action, list):
            self.robot.angle1 += action[0]
            self.robot.angle2 += action[1]
            self.robot.angle3 += action[2]
        elif isinstance(action, dict):
            self.robot.angle1 += action["dangle1"]
            self.robot.angle2 += action["dangle2"]
            self.robot.angle3 += action["dangle3"]
        self.robot._update()

