A Journey to Ghimo through Tutorials
=====================================

In this section I collected a sequence of tutorials that allows the reader to understand the main topics included in Ghimo and how they are addressed.
The first tutorials are pretty basic, an experienced robotic professional or scientist knows about them since years, but the main goal of this journey
is to *build* the framework, gathering the problems to be addressed and the related solutions, generalizing them to finally obtain the framework.
For this reason, I suggest to go through this journey even if you are an expert in the field (just skip definitions and explanations you already know,
you can read only the part related to Ghimo).

First steps
-----------

Checkerboard world (Ghimo internal simulator)
  In this very first example, we have an agent that lives in a checkerboard world and it has to move from an initial position to a goal position.
  The main aim of this very easy example is to have an overview of the Ghimo framework.

  Checkerboard: random agent
  Checkerboard: random agent with sensors
  Checkerboard: agent with goal

Planar omnidirectional vehicle, naive controller (Ghimo internal simulator)
  In this example, the robot is free to move continuously in the environment: its goal is to reach a specific position in that environment.

  Planar omnidirectional agent: random agent (brownian motion)
  Planar omnidirectional agent: random agent with sensors
  Planar omnidirectional agent: agent with goal

First detour: cellular automata

  Cellular automata: fireworks
  Cellular automata: Escherichia Coli
  Cellular automata: genetic algorithms

Planar non-holonomic robot, FIXME Oriolo controller (Ghimo internal simulator)
  The non-holonomic constraints model a robot that is not able to move sideways with respect to the direction of its wheels.
  Cite Oriolo controller FIXME
  Multi-robot with different parameters
  NASA controller Pure Pursuit
  Naive controller

Planar non-holonomic robot, Dynamic Window Approach (Ghimo internal simulator)
  Trajectory rollout

Planar car-like robot, Probabilistic RoadMap (PRM) (Ghimo internal simulator)

Planar car-like robot, Rapid-exploring Random Trees (RRT) (Ghimo internal simulator)

Planar 3-links manipulator, RRT and variants (Ghimo internal simulator)

Planar 4-links manipulator, RRT and variants (Ghimo internal simulator)

Using different simulators
--------------------------

Planar non-holonomic robot, DWA (Stage simulator)

Planar non-holonomic robot, DWA (PyBullet simulator)

Planar non-holonomic robot, DWA (Gazebo simulator)

Planar non-holonomic robot, DWA (Webots simulator)
