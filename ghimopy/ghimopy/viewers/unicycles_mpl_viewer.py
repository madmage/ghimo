import matplotlib as mpl
from ghimopy.viewers.mpl_viewer import MplViewer


class UnicyclesMplViewer(MplViewer):
    def __init__(self, width, height):
        super().__init__(width, height)
        self.unicycles = {}

    def _add_unicycle(self, name):
        unicycle = {
            "mpl_unicycle": mpl.patches.PathPatch(mpl.path.Path([(0.5, 0.0), (0.0, 0.2), (0.0, -0.2), (0.5, 0.0)]), facecolor="red", edgecolor="black"),
            "mpl_goal": mpl.patches.PathPatch(mpl.path.Path([(0.5, 0.0), (0.0, 0.2), (0.0, -0.2), (0.5, 0.0)]), edgecolor="green"),
        }
        self.unicycles[name] = unicycle

    def render_objects(self) -> None:
        for agent_name in self.environment.agents:
            if agent_name not in self.unicycles:
                self._add_unicycle(agent_name)
        
        for unicycle_name, unicycle in self.unicycles.items():
            x, y, theta = self.environment.agents[unicycle_name]["state"]
            unicycle["mpl_unicycle"].set_transform(mpl.transforms.Affine2D().rotate(theta).translate(x, y) + self._ax.transData)
            self._ax.add_patch(unicycle["mpl_unicycle"])
            if "goal" in self.environment.agents[unicycle_name] and self.environment.agents[unicycle_name]["goal"] is not None:
                x, y, theta = self.environment.agents[unicycle_name]["goal"]
                unicycle["mpl_goal"].set_transform(mpl.transforms.Affine2D().rotate(theta).translate(x, y) + self._ax.transData)
                self._ax.add_patch(unicycle["mpl_goal"])

