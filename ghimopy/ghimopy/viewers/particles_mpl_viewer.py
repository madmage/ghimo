from matplotlib import pyplot as plt
from ghimopy.viewers.mpl_viewer import MplViewer


class ParticlesMplViewer(MplViewer):
    def __init__(self, width, height):
        super().__init__(width, height)
        self.particle_positions = {}

    def render_objects(self) -> None:
        for agent_name, agent in self.environment.agents.items():
            self.particle_positions[agent_name] = agent["state"]

        plt.scatter(
            [e[0] for e in self.particle_positions.values()],
            [e[1] for e in self.particle_positions.values()]
        )

