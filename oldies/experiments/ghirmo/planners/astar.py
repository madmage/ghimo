class PriorityQueue:
    def __init__(self):
        self.queue = []

    def push(self, item, value):
        self.queue.append([item, value])
        self.queue.sort(key=lambda x: x[1])

    def pop(self):
        return self.queue.pop(0)

    def get_value(self, item):
        return [v[1] for v in self.queue if v[0] == item][0]

    def update_value(self, item, value):
        for s in self.queue:
            if s[0] == item:
                s[1] = value
                break

    def __contains__(self, item):
        return item in (x[0] for x in self.queue)

    def empty(self):
        return len(self.queue) == 0


class AStar:
    def __init__(self):
        self.info = {}

    def _extract_plan(self, to_reach, cur_state):
        plan = []
        plan.append((cur_state, None))  # goal, no action afterwards
        while cur_state:
            for ss in to_reach:
                if ss[2] == cur_state:
                    if ss[0]:
                        plan.append((ss[0], ss[1]))  # state, action
                    cur_state = ss[0]
                    break
        plan.reverse()
        return plan

    def _goal_reached(self, state, goal):
        pass

    def _generate_actions(self, state):
        pass

    def _forward_action(self, state, action):
        pass

    def compute_plan(self, state, goal):
        open_pq = PriorityQueue()
        open_pq.push(state, 0)
        visited = []
        to_reach = [(None, None, state)]
        for _ in range(10000):
            if open_pq.empty():
                # failed
                return []
            cur_state, cur_cost = open_pq.pop()
            visited.append(cur_state)
            if self._goal_reached(cur_state, goal):
                return self._extract_plan(to_reach, cur_state)

            actions = self._generate_actions(cur_state)
            for action in actions:
                new_state = self._forward_action(cur_state, action)
                new_cost = cur_cost + 1
                if new_state in visited:
                    continue
                elif new_state in open_pq:
                    old_cost = open_pq.get_value(new_state)
                    if new_cost < old_cost:
                        open_pq.update(new_state, new_cost)
                        idx_in_to_reach = [i for i, v in enumerate(to_reach) if v[2] == new_state][0]
                        to_reach[idx_in_to_reach] = (cur_state, action, new_state)
                else:
                    open_pq.push(new_state, new_cost)
                    to_reach.append((cur_state, action, new_state))

        return []    
