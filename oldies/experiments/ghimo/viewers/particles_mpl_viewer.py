from matplotlib import pyplot as plt
from ghimo.viewers.mpl_viewer import SimpleMplViewer


class ParticlesMplViewer(SimpleMplViewer):
    def __init__(self, width, height):
        super().__init__(width, height)
        self.particle_positions = []

    def render_objects(self) -> None:
        plt.scatter(
            [e[0] for e in self.particle_positions],
            [e[1] for e in self.particle_positions]
        )

