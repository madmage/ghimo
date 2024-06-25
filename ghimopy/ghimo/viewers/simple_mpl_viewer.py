from matplotlib import pyplot as plt
from ghimo.viewers.viewer_base import ViewerBase


class SimpleMplViewer(ViewerBase):
    def __init__(self, width, height):
        plt.ion()
        self._fig, self._ax = plt.subplots()
        plt.show()
        self._width = width
        self._height = height

    def render(self) -> bool:
        if not plt.fignum_exists(self._fig.number):
            return False
        self._ax.cla()
        self._ax.grid()
        self._ax.axis("equal")
        self._ax.set_xlim([-self._width / 2.0, self._width / 2.0])
        self._ax.set_ylim([-self._height / 2.0, self._height / 2.0])
        self._ax.set_aspect("equal", "box")
        self.render_objects()
        plt.pause(0.01)
        return True

    def render_objects(self) -> None:
        pass

