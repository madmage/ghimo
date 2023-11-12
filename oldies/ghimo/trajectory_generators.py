import numpy as np
from ghimo.geometry import Circle, Rectangle, Line, load_vector_map, dist2
from ghimo.graphics_handlers import MatplotlibGraphicsHandler, trajectory_np2gfx
from ghimo.control_utils import simulate

# states here are tuples or np.arrays containing: (x, y, theta, v, w)
# trajectories are lists or np.arrays of states, augmented with delta_t: [(x, y, theta, v, w, dt)]

class BaseTrajectoryGenerator:
    def __init__(self):
        pass

    def set_initial_state(self, initial_state):
        pass

    def set_goal(self, goal):
        pass

    def set_collision_checker(self, collision_checker):
        pass

    def update(self):
        pass

    def get_best_trajectory(self):
        pass

    def get_other_trajectories(self):
        pass


class RandomTrajectoryGenerator(BaseTrajectoryGenerator):
    def __init__(self):
        self.best_value = 0.0
        self.traj_steps = 10
        self.state_size = 6
        self.best_traj = np.zeros((self.traj_steps, self.state_size))
        self.cur_traj = np.zeros((self.traj_steps, self.state_size))
        self.initial_state = None
        self.goal = None
        self.collision_checker = None

    def set_initial_state(self, initial_state):
        self.initial_state = initial_state
        self.best_value = 0.0

    def set_goal(self, goal):
        self.goal = goal
        self.best_value = 0.0

    def set_collision_checker(self, collision_checker):
        self.collision_checker = collision_checker

    def update(self):
        new_traj = np.zeros((self.traj_steps, self.state_size))  # x, y, theta, v, w, dt
        new_traj[0] = (self.initial_state[0], self.initial_state[1], self.initial_state[2], 0.0, 0.0, 0.0)
        for i in range(1, self.traj_steps):
            v = np.clip(self.best_traj[i][3] + (np.random.rand() * 0.2 - 0.1), 0, 2.5)
            w = self.best_traj[i][4] + (np.random.rand() * 0.2 - 0.1)
            dt = np.clip(self.best_traj[i][5] + (np.random.rand() * 0.08 - 0.04), 0.0, 2.0)
            nx, ny, nth = simulate(new_traj[i-1], (v, w), dt)
            new_traj[i] = (nx, ny, nth, v, w, dt)
        
        if self.collision_checker is not None and self.collision_checker.trajectory_collide(new_traj):
            return

        self.cur_traj = new_traj

        new_value = self.goal.state_value((new_traj[-1][0], new_traj[-1][1]))
        if new_value > self.best_value:
            self.best_value = new_value
            self.best_traj = new_traj

    def get_best_trajectory(self):
        return self.best_traj

    def get_other_trajectories(self):
        return [self.cur_traj]


class DWATrajectoryGenerator(BaseTrajectoryGenerator):
    def __init__(self):
        self.initial_state = None
        self.goal = None
        self.collision_checker = None
        self.v_limits = [-1.0, 1.0]
        self.w_limits = [-0.5, 0.5]
        self.v_steps = np.linspace(-0.2, 0.2, 5)
        self.w_steps = np.linspace(-0.1, 0.1, 5)
        self.traj_steps = 10
        self.delta_t = 1.0
        self.state_size = 6
        self.cur_trajs = []
        self.best_traj = None

    def set_initial_state(self, initial_state):
        self.initial_state = initial_state

    def set_goal(self, goal):
        self.goal = goal

    def set_collision_checker(self, collision_checker):
        self.collision_checker = collision_checker

    def update(self):
        trajs = []
        for dv in self.v_steps:
            for dw in self.w_steps:
                traj = np.zeros((self.traj_steps, self.state_size))
                traj[0] = np.concatenate((self.initial_state, [0, 0, 0]))
                v = 0.0 + dv
                w = 0.0 + dw
                for t in range(self.traj_steps - 1):
                    x, y, theta, _, _, _ = traj[t]
                    nx, ny, ntheta = simulate((x, y, theta), (v, w), self.delta_t)
                    traj[t + 1] = np.array((nx, ny, ntheta, v, w, self.delta_t))
                trajs.append(traj)
        self.cur_trajs = trajs
        best_value = 0.0
        for traj in self.cur_trajs:
            new_value = self.goal.state_value((traj[-1][0], traj[-1][1]))
            if new_value > best_value:
                self.best_traj = traj
                best_value = new_value

    def get_best_trajectory(self):
        return self.best_traj

    def get_other_trajectories(self):
        return self.cur_trajs


class RRTTrajectoryGenerator(BaseTrajectoryGenerator):
    def __init__(self):
        pass

    def set_initial_state(self, initial_state):
        pass

    def set_goal(self, goal):
        pass

    def set_collision_checker(self, collision_checker):
        pass

    def update(self):
        pass

    def get_best_trajectory(self):
        pass

    def get_other_trajectories(self):
        pass
