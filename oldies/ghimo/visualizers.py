import numpy as np
import matplotlib.pyplot as plt
import matplotlib

from ghimo.geometry import Circle, Rectangle, Line, Path


class Visualizer:
    def __init__(self):
        pass

    def add_object(self, obj):
        pass

    def set_objects(self, objs):
        pass

    def update_object(self, name, objdef):
        pass


class MatplotlibVisualizer(Visualizer):
    def __init__(self):
        fig, ax = plt.subplots()
        fig.canvas.manager.set_window_title("Ghimo interactive figure")
        plt.ion()
        plt.show()

        self.ax = ax
        self.objs = {}
        self.mpl_patches = {}
        self.ax.set_aspect('equal')
        #self.ax.set_xlim([-10, 10])
        #self.ax.set_ylim([-10, 10])
        self.ax.set_xticks(np.arange(-10.0, 11.0, 2))
        self.ax.set_yticks(np.arange(-10.0, 11.0, 2))
        self.ax.grid(color='gray', ls='--', lw=0.5)

        fig.canvas.mpl_connect('key_press_event', lambda e: print(f"Key press c {repr(e)}"))
        plt.connect('key_press_event', lambda e: print(f"Key press p {repr(e)}"))
        fig.canvas.mpl_connect('button_press_event', lambda x: print("AAA"))
        plt.connect('button_press_event', lambda e: print(f"AAA {e}"))

    def render(self):
        plt.pause(0.0001)

    def clear_objects(self):
        self.objs = {}

    def add_objects(self, objs):
        for obj in objs:
            self.add_object(obj)

    def add_object(self, obj):
        self.objs[obj['id']] = obj
        patch = None
        if isinstance(obj, Circle):
            patch = matplotlib.patches.Circle((obj['cx'], obj['cy']), obj['r'])
            props = {}
            if 'layer' in obj.keys():
                if obj['layer'] == 'obstacles':
                    props = {'fc': 'black', 'ec': 'black', 'lw': 1.0, 'ls': '-'}
            props.update({k: v for k, v in obj.items() if k in ['fc', 'ec', 'lw', 'ls']})
            patch.update(props)
        if isinstance(obj, Rectangle):
            patch = matplotlib.patches.Rectangle((obj['left'], obj['top']), obj['right'] - obj['left'], obj['bottom'] - obj['top'])
            props = {}
            if 'layer' in obj.keys():
                if obj['layer'] == 'obstacles':
                    props = {'fc': 'black', 'ec': 'black', 'lw': 1.0, 'ls': '-'}
                if obj['layer'] == 'goal':
                    props = {'fc': 'limegreen', 'ec': 'limegreen', 'lw': 0.0}
            props.update({k: v for k, v in obj.items() if k in ['fc', 'ec', 'lw', 'ls']})
            patch.update(props)
        if isinstance(obj, Line):
            patch = matplotlib.lines.Line2D([obj["x1"], obj["y1"]], [obj["x2"], obj["y2"]])
            props = {}
            if 'layer' in obj.keys():
                if obj['layer'] == 'obstacles':
                    props = {'fc': 'black', 'ec': 'black', 'lw': 1.0, 'ls': '-'}
                if obj['layer'] == 'goal':
                    props = {'fc': 'limegreen', 'ec': 'limegreen', 'lw': 0.0}
            props.update({k: v for k, v in obj.items() if k in ['c', 'lw', 'ls']})
            patch.update(props)
        if isinstance(obj, Path):
            path = matplotlib.path.Path(obj["coords"])
        if patch is not None:
            self.mpl_patches[obj['id']] = patch
            print(type(obj))
            print(isinstance(obj, Line))
            if isinstance(obj, Line) or isinstance(obj, Line2D):
                print("mmm")
                self.ax.add_line(patch)
            elif isinstance(obj, Path):
                self.ax.add_
            #elif isinstance(obj, Lines):
            #    self.ax.add_collection(patch)
            else:
                self.ax.add_patch(patch)

    def update_object(self, name, data):
        obj = self.objs[name]
        for k, v in data.items():
            obj[k] = v

        patch = self.mpl_patches[name]
        if isinstance(obj, Circle):
            if obj.is_changed(['cx', 'cy']):
                patch.center = obj['cx'], obj['cy']
        elif isinstance(obj, Rectangle):
            if obj.is_changed(['left', 'top']):
                patch.set_xy((obj['left'], obj['top']))
            if obj.is_changed(['left', 'top', 'right', 'bottom']):
                patch.set_width(obj['right'] - obj['left'])
                patch.set_height(obj['bottom'] - obj['top'])
        elif isinstance(obj, Line):
            if obj.is_changed("x1") or obj.is_changed("x2"):
                patch.set_xdata([obj["x1"], obj["x2"]])
            if obj.is_changed("y1") or obj.is_changed("y2"):
                patch.set_ydata([obj["y1"], obj["y2"]])
        #elif isinstance(obj, Lines):
        #    if obj.is_changed('lines_xys'):
        #        a = obj['lines_xys']
        #        if not isinstance(a, np.ndarray):
        #            a = np.array(a)
        #        print(a.shape)
        #        patch.set_offsets(a)


def trajectory_np2gfx(nptraj):
    print(type(nptraj))
    if isinstance(nptraj, np.ndarray):
        print(nptraj.shape)
    gfxtraj = {'xys': nptraj}
    gfxtraj = {'lines_xys': nptraj}
    return gfxtraj
