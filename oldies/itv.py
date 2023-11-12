#!/usr/bin/env python3
import numpy as np
import matplotlib
import matplotlib.pyplot as plt

fig, ax = plt.subplots()
line, = ax.plot([1, 2, 3, 2])
plt.ion()
plt.show()

should_pause = False

def on_keypress(event):
    global should_pause
    print("EVT KEY" + event.key)
    if event.key == 'x':
        print("exiting")
        should_pause = True

fig.canvas.mpl_connect('key_press_event', on_keypress)
plt.connect('key_press_event', lambda e: print(f"Key press {e}"))
fig.canvas.mpl_connect('button_press_event', lambda x: print("AAA"))
plt.connect('button_press_event', lambda e: print(f"AAA {e}"))

while True:
    while not should_pause:
        line.set_ydata(np.random.random(len(line.get_xdata()))+1)
        plt.pause(0.01)
    should_pause = False
    cmd = input("Interactive trajectory viewer> ")
    if cmd == 'exit':
        break
