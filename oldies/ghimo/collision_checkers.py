from ghimo.geometry import Circle, Rectangle, Line, dist2

class PointRobotCollisionChecker:
    def __init__(self):
        self.objects = []

    def set_objects(self, objects):
        self.objects = objects

    def trajectory_collide(self, traj):
        for i in range(traj.shape[0]):
            if self.state_collide(traj[i]):
                return True
        return False

    def state_collide(self, state):
        for obj in self.objects:
            if isinstance(obj, Circle):
                if dist2(obj['cx'], obj['cy'], state[0], state[1]) < obj['r'] ** 2:
                    return True
            elif isinstance(obj, Rectangle):
                if (state[0] >= obj['x0'] and state[0] <= obj['x1'] and
                    state[1] >= obj['y0'] and state[1] <= obj['y1']):
                    return True
            elif isinstance(obj, Line):
                pass
        return False
