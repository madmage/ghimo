import matplotlib as mpl
from ghimo.viewers.simple_mpl_viewer import SimpleMplViewer


class UnicycleMplViewer(SimpleMplViewer):
    def __init__(self, width, height):
        super().__init__(width, height)
        self._mpl_unicycle = mpl.patches.PathPatch(mpl.path.Path([(0.5, 0.0), (0.0, 0.2), (0.0, -0.2), (0.5, 0.0)]), facecolor="red", edgecolor="black")
        self._mpl_goal = mpl.patches.PathPatch(mpl.path.Path([(0.5, 0.0), (0.0, 0.2), (0.0, -0.2), (0.5, 0.0)]), edgecolor="green")
        self.unicycle_pose = [0, 0, 0]
        self.goal_pose = None

    def render_objects(self) -> None:
        x, y, theta = self.unicycle_pose
        self._mpl_unicycle.set_transform(mpl.transforms.Affine2D().rotate(theta).translate(x, y) + self._ax.transData)
        self._ax.add_patch(self._mpl_unicycle)
        if self.goal_pose is not None:
            x, y, theta = self.goal_pose
            self._mpl_goal.set_transform(mpl.transforms.Affine2D().rotate(theta).translate(x, y) + self._ax.transData)
            self._ax.add_patch(self._mpl_goal)

