import time
from ghimo.viewers.viewer import Viewer


class GridEnvironmentConsoleViewer(Viewer):
    def __init__(self, wait_time=0.0, clear_screen=False):
        self.wait_time = wait_time
        self.clear_screen = clear_screen

    def render(self):
        if self.clear_screen:
            print("\033[2J")

        for y in range(0, self.environment.height):
            for x in range(0, self.environment.width):
                draw_cell = True
                for agent_name, agent in self.environment.agents.items():
                    if agent["state"][0] == x and agent["state"][1] == y:
                        print("\033[1;96;44mL\033[0m", end="")
                        draw_cell=False
                        break
                    if agent.get("info", None) and agent["info"].get("goal", None) and [x, y] == agent["info"]["goal"]:
                        print("\033[1;96;44mG\033[0m", end="")
                        draw_cell=False
                        break
                    if agent.get("info", None) and agent["info"].get("path", None) and [x, y] in [p[0] for p in agent["info"]["path"]]:
                        print("\033[1;34m*\033[0m", end="")
                        draw_cell=False
                if draw_cell:
                    print("·" if self.environment.cells[x][y] == "0" else "▮", end="")
            print("")
        print("")
        time.sleep(self.wait_time)
