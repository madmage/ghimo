#!/usr/bin/env python3
import math
import numpy as np
from matplotlib import pyplot as plt
import matplotlib as mpl

plt.ion()
fig, ax = plt.subplots()
plt.show()


class Path:
    def __init__(self, coords, **attrs):
        self.coords = coords
        self.__dict__.update(**attrs)


rect = mpl.patches.Rectangle((-0.25, -0.2), 0.5, 0.4, facecolor="blue", edgecolor="black")
arrow = mpl.patches.PathPatch(mpl.path.Path([(0.2, 0.0), (0.0, 0.1), (0.0, -0.1), (0.2, 0.0)]), facecolor="red", edgecolor="black")
robot = mpl.collections.PatchCollection([rect, arrow], match_original=True)

a = 0.0
for _ in range(1000):
    ax.cla()
    ax.grid()
    ax.axis("equal")
    ax.set_xlim([-5.0, 5.0])
    ax.set_ylim([-5.0, 5.0])

    coords = np.random.rand(10, 2) + [1.0, 0.0]
    path = Path(coords)
    ax.plot(path.coords[:, 0], path.coords[:, 1])

    x = math.cos(a) * 2.0 - 2.0
    y = math.sin(a) * 2.0
    theta = a + math.pi / 2.0
    robot.set_transform(mpl.transforms.Affine2D().rotate(theta).translate(x, y) + ax.transData)
    ax.add_collection(robot)

    a += 0.01
    plt.pause(0.01)
    if not plt.fignum_exists(fig.number):
        break

plt.close()
