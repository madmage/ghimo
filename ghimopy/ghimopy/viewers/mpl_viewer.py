from matplotlib import pyplot as plt
from ghimopy.viewers.viewer import Viewer


class MplViewer(Viewer):
    def __init__(self, width, height, wait_time=0.01):
        super().__init__()
        plt.ion()
        self._fig, self._ax = plt.subplots()
        plt.show()
        self._width = width
        self._height = height
        self._wait_time = wait_time
        self.exit_requested = False

    def render(self) -> None:
        if not plt.fignum_exists(self._fig.number):
            self.exit_requested = True
            return
        self._ax.cla()
        self._ax.grid()
        self._ax.axis("equal")
        self._ax.set_xlim([-self._width / 2.0, self._width / 2.0])
        self._ax.set_ylim([-self._height / 2.0, self._height / 2.0])
        self._ax.set_aspect("equal", "box")
        self.render_objects()
        plt.pause(self._wait_time)

    def render_objects(self) -> None:
        pass
