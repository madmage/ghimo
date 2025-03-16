import math

def angle_diff(a0, a1):
    return math.atan2(math.sin(a0 - a1), math.cos(a0 - a1))
