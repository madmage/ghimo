import re
import json


class GeometryObject2D:
    def __init__(self, data):
        self.data = data
        self.reset_changed()

    def __getitem__(self, key):
        return self.data[key]

    def __setitem__(self, key, val):
        self.data[key] = val
        self.changed.add(key)

    def is_changed(self, keys):
        if isinstance(keys, str):
            return keys in self.changed
        elif isinstance(keys, list):
            return set(keys).intersection(set(self.changed))

    def reset_changed(self):
        self.changed = set()

    def items(self):
        return self.data.items()

    def keys(self):
        return self.data.keys()


class Circle(GeometryObject2D):
    def __init__(self, data):
        cdata = {'cx': 0, 'cy': 0, 'r': 0}
        cdata.update(data)
        super().__init__(cdata)


class Rectangle(GeometryObject2D):
    def __init__(self, data):
        cdata = {'left': 0, 'top': 0, 'right': 0, 'bottom': 0}
        cdata.update(data)
        super().__init__(cdata)


class Line(GeometryObject2D):
    def __init__(self, data):
        cdata = {"x1": 0, "y1": 0, "x2": 0, "y2": 0}
        cdata.update(data)
        super().__init__(cdata)


class Lines(GeometryObject2D):
    def __init__(self, data):
        cdata = {"lines_xys": []}
        cdata.update(data)
        super().__init__(cdata)


class Path(GeometryObject2D):
    def __init__(self, data):
        cdata = {"coords": []}
        cdata.update(data)
        super().__init__(cdata)


def load_vector_map(filename):
    classname_to_class = {"Circle": Circle, "Rectangle": Rectangle, "Line": Line, "Path": Path}
    vmap = {'objects': []}
    with open(filename, 'r') as f:
        for line in f:
            objtype, objdef = re.match(r"([\w]+) *(.*)", line).groups()
            objdef = json.loads(objdef)
            cls = classname_to_class[objtype]
            obj = cls(objdef)
            vmap['objects'].append(obj)
    return vmap


def dist2(a, b):
    if len(a) != len(b):
        raise ValueError("The length of the two lists must match")
    return sum([(ax - bx) ** 2.0 for ax, bx in zip(a, b)])
