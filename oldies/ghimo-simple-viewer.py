#!/usr/bin/env python3
import math
import re
import json
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import socketserver
import threading

from ghimo.geometry import Circle, Rectangle, Line, Lines, load_vector_map, dist2
from ghimo.graphics_handlers import MatplotlibGraphicsHandler, trajectory_np2gfx
from ghimo.control_utils import simulate
from ghimo.trajectory_generators import RandomTrajectoryGenerator, DWATrajectoryGenerator
from ghimo.collision_checkers import PointRobotCollisionChecker

fig, ax = plt.subplots()
fig.canvas.set_window_title("Ghimo interactive figure")
plt.ion()
plt.show()

should_pause = False


def on_keypress(event):
    global should_pause
    print("EVT KEY " + event.key)
    if event.key == 'x':
        print("exiting")
        should_pause = True


class CommandRequestHandler(socketserver.BaseRequestHandler):
    def handle(self):
        data = str(self.request.recv(1024), 'ascii')
        cur_thread = threading.current_thread()
        response = bytes("{}: {}".format(cur_thread.name, data), 'ascii')
        self.request.sendall(response)


class CommandTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
    def __init__(self, server_address, RequestHandlerClass):
        self.allow_reuse_address = True
        super(CommandTCPServer, self).__init__(server_address, RequestHandlerClass)


fig.canvas.mpl_connect('key_press_event', on_keypress)
plt.connect('key_press_event', lambda e: print(f"Key press {e}"))
fig.canvas.mpl_connect('button_press_event', lambda x: print("AAA"))
plt.connect('button_press_event', lambda e: print(f"AAA {e}"))

server = CommandTCPServer(('localhost', 4345), CommandRequestHandler)
server_thread = threading.Thread(target=server.serve_forever)
server_thread.daemon = True
server_thread.start()

gfx = MatplotlibGraphicsHandler(ax)
vmap = load_vector_map("example2.vmap")
gfx.set_objects(vmap['objects'])

gfx.add_object(Line({'id': 'start_pose', 'xys': [[0, -8.0], [0.1, -8.0]], 'c': 'red'}))
gfx.add_object(Line({'id': 'best_traj', 'xys': [], 'c': 'green', 'lw': 2}))
gfx.add_object(Lines({'id': 'cur_trajs', 'lines_xys': [], 'c': 'gray', 'lw': 1}))

gfx.add_object(Lines({'id': 'test_lines', 'lines_xys': [[[0, 0], [0, 1]], [[2, 3], [4, 5]]]}))


class Goal:
    def __init__(self):
        pass

    def state_value(self, state):
        pass


class PointGoal(Goal):
    def __init__(self, state):
        self.goal_state = state
        self.accepted_dist = 1.0
        self.max_dist = 1000.0

    def state_value(self, state):
        d2 = dist2(self.goal_state[0], self.goal_state[1], state[0], state[1])
        # linearly maps [self.max_dist, self.accepted_dist] to [0.0, 0.5]
        # and [self.accepted_dist, 0] to [0.5, 0.1]
        return np.interp(d2, [0.0, self.accepted_dist ** 2, self.max_dist ** 2], [1.0, 0.5, 0.0])


#traj_gen = RandomTrajectoryGenerator()
traj_gen = DWATrajectoryGenerator()
traj_gen.set_initial_state((0.0, -8.0, 0.0))
traj_gen.set_goal(PointGoal((1.5, 3.5)))

collision_checker = PointRobotCollisionChecker()
collision_checker.set_objects(vmap['objects'])
traj_gen.set_collision_checker(collision_checker)

while True:
    while not should_pause:
        traj_gen.update()
        new_trajs = traj_gen.get_other_trajectories()
        best_traj = traj_gen.get_best_trajectory()
        gfx.update_object('cur_trajs', trajectory_np2gfx(new_trajs))
        gfx.update_object('best_traj', trajectory_np2gfx(best_traj))
        plt.pause(0.01)

    should_pause = False
    cmd = input("Interactive trajectory viewer> ")
    if cmd == 'exit':
        break

server.shutdown()

# planner: something that plans n steps in advance
# inputs: starting robot state, final and partial goals
# outputs: trajectory (includes v, w and dt)

# trajectory: a np.array with n_steps rows and a number of columns = state + control
# goal: an object with a description (e.g. a point, a pose, a pose/point with tolerance, a shape, a shape with fuzzy optimality value), a function to compute the goodness of a state
# pose: x, y, theta
# state: x, y, theta, v, w, etc.
# control: v, w, dt, or accelerations, etc.

# DWA:
# a planner computes a set of trajectories, each with a fixed v, w and all with the same dt
# an optimality principle is used to choose a trajectory
# the first control of the trajectory is executed

# RTT based:
# a planner computes a tree of trajectories, with fixed dt and varying v and w
# an optimality principle choose the branch of the tree
# an executor follows the trajectory by executing (open-loop or closed-loop) the controls
