#!/usr/bin/env python3
import math
import time

# this has been taken from https://gist.github.com/bradphelan/7fe21ad8ebfcb43696b8
TEST_CASES = [
    # given [a0, a1, ar] it should be that a0 - a1 == ar
    [0.2,               0.1,                0.1],
    [0.2 + 2 * math.pi, 0.1,                0.1],
    [0.2 - 2 * math.pi, 0.1,                0.1],
    [0.2,               0.1 + 2 * math.pi,  0.1],
    [0.2,               0.1 - 2 * math.pi,  0.1],
    [0.1,               0.2,               -0.1],
    [0.1 + 2 * math.pi, 0.2,               -0.1],
    [0.1 - 2 * math.pi, 0.2,               -0.1],
    [0.1,               0.2 + 2 * math.pi, -0.1],
    [0.1,               0.2 - 2 * math.pi, -0.1],
]


# see thread https://stackoverflow.com/questions/1878907/how-can-i-find-the-smallest-difference-between-two-angles-around-a-point

# time computation, see https://superfastpython.com/benchmark-execution-time/

def ang_diff_atan2(a0, a1):
    return math.atan2(math.sin(a0 - a1), math.cos(a0 - a1))


def ang_diff_min(a0, a1):
    return min(2 * math.pi - math.fabs(a0 - a1), math.fabs(a0 - a1))


def ang_diff_fmod(a0, a1):
    return math.fmod(a0 - a1 + math.pi, 2 * math.pi) - math.pi


def ang_diff_min_trick(a0, a1):
    return min(a0 - a1, a0 - a1 + 2 * math.pi, a0 - a1 - 2 * math.pi, key=abs)


for fn in [ang_diff_atan2, ang_diff_min, ang_diff_fmod, ang_diff_min_trick]:
    print(f"Evaluating {fn.__name__}:")
    for test in TEST_CASES:
        t0 = time.process_time()
        for _ in range(1000):
            cr = fn(test[0], test[1])
        t1 = time.process_time()
        ok = "OK" if math.fabs(cr - test[2]) < 0.001 else "DIFFERENT"
        tm = (t1 - t0) * 1e6 / 1000
        print(f"  test {test[0]:.3f} - {test[1]:.3f} == (expected: {test[2]:.3f}, computed: {cr:.3f}) {ok} [{tm:.3f} ms]")
