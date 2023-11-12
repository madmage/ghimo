class GenericPlanner:
    def __init__(self):
        pass



class RapidexploringRandomTree:
    def __init__(self):
        self.cur_state = None
        self.goal = None
        self.tree = Tree()

    def plan(self):
        self.tree.clear()
        self.tree.add_vertex(self.cur_state)
        while should_continue:
            q_rand = self.generate_extension_target()
            q_near = self.nearest_vertex(q_rand, self.tree)
            q_new = self.extend(q_near, q_rand, delta_q)
            self.tree.add_vertex(q_new)
            self.tree.add_edge(q_near, q_new)


class AStar:
    def __init__(self):
        pass

    def plan(self):
        open_pq = PriorityQueue()
        closed = []
        planning_return_structure = ...

