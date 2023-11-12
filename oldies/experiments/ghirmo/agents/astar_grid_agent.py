from ..planners.grid_astar import GridAStar


class AStarGridAgent:
    def __init__(self, static_info):
        self.env_width = static_info["env_width"]
        self.env_height = static_info["env_height"]
        self.planner = GridAStar()
        self.planner.info["grid_width"] = static_info["env_width"]
        self.planner.info["grid_height"] = static_info["env_height"]

    def compute_action(self, obs):
        self.planner.info["cells"] = obs["cells"]
        plan = self.planner.compute_plan(obs["state"], obs["goal"])
        return plan[0][1] if plan else None

    def process_feedback(self, feedback) -> None:
        pass


