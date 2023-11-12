import math

def simulate(robot_pose, control_input, delta_t):
    rx = robot_pose[0] + control_input[0] * delta_t * math.cos(robot_pose[2])
    ry = robot_pose[1] + control_input[0] * delta_t * math.sin(robot_pose[2])
    rth = robot_pose[2] + control_input[1] * delta_t
    return rx, ry, rth