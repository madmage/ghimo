Checkerboard world - the very first tutorial
============================================

This tutorial is very simple, but it is actually an excuse to introduce many Ghimo topics. First of all, Ghimo is a framework for robot motion
in an environment. A robot perceives the environment through its sensors (laser scanners, normal and 3D cameras, proximity sensor, etc.) and
perform actions through actuators (wheel motors for a mobile robot or joint motors for a manipulator... or both).

left to right direction
```kroki-plantuml
rectangle world
rectangle robot

world -r--> (sensors)
(sensors) -r--> robot : sensor data
robot -r--> (actuators) : actions
(actuators) -l--> world
```

We could add a component between the raw sensor data and Ghimo, that is responsible to build and keep up-to-date an environment representation,
and we can also add additional controllers (software or hardware) that make the movements commanded by Ghimo more precise or simply feasible for the underlying hardware.

```kroki-plantuml
rectangle world
rectangle robot
usecase sensors
usecase actuators
usecase "environment representation" as envrepr
usecase controllers

world -u-> (sensors)
(sensors) -> envrepr: sensor data
envrepr -u-> robot: environment
(sensors) -u-> robot : sensor data
robot -d-> (actuators) : actions
(actuators) -d-> world
```

In this first tutorial, the world is simply a grid where a cell can be either free or occupied by an obstacle and where the robot can be in any of the free cells.
It can sense the whole environment (i.e. all the cells), the whole-environment-sensor gives reliable data: if it says a cell is free or occupied, we can trust it.
The robot set of actions is discrete: up, down, right, left, with obvious meaning. Actions are deterministic, meaning that if the robot decide to go up, this will happen.

The following elements will be present in all tutorials and are the main building blocks of any Ghimo system:

- the environment
- the sensors
- the actuators
- the robot state
- the environment state
- the robot model: given a robot state, an environment state, a control and a duration, returns the possible robot and environment states resulting by applying the control for that given duration

```kroki-blockdiag no-transparency=false
blockdiag {
  blockdiag -> generates -> "block-diagrams";
  blockdiag -> is -> "very easy!";

  blockdiag [color = "greenyellow"];
  "block-diagrams" [color = "pink"];
  "very easy!" [color = "orange"];
}
```

In addition to the above elements, the following objects are often used in Ghimo:

- a trajectory is a sequence of alternating robot/environment states and control actions duration pairs
