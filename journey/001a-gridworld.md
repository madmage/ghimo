# Basic concepts

We start this journey by defining the two main object that we are going to use
in our experiments. Our aim is to design such objects as general as possible,
so that we can use the same structure for different environments, implement different
algorithms and test different situations.

The main objects are: an environment and a set of agents.
The **environment** represents the part of the world that we are interested to,
it can be either a very simple or a very complex simulation, or even a part
of the real world. An environment **contains** a set of **agents** and is
responsible to keep track of the natural changes that occurs, as well as the
changes due to the actions of the agents. An environment usually includes the
representation of an internal state, that include also the state of the single
agent living within.
The **agents**, often only one in most of the experiments, are entities that
live in the environment and are able to interact with it. In particular, the
agents are provided with a set of **observations** that are taken from the environment,
and can take choices and modify the environment using their **actions**.

The interface between the agents and the environment in which they live is provided
by an **environment interface** that is responsible to provide the observations
to the agents and to communicate to the environment the actions that the agents
want to perform.

```mermaid
graph LR
E[environment] -- observation --> I@{shape: fork, label: interface}
I -- observation --> A
A[agent] -- action --> I
I -- action --> E
```

All experiments will be performed using discrete time steps, where in each time
step the agents are provided with their observations and return their intended
actions, and the environment is responsible to keep track of the changes in its
state.

In pseudo-Python code, every experiment will have this structure:

```
env = AnEnvironment()
agent = AnAgent()
interface = AnInterface(env, agent)

env.reset()
while not experiment_finished():  # this could be either a predefined time or will stop if a particular condition is met
  agent.step()
  env.step()
```

where the step of the agent is something like:
```
class AnAgent:
  def step(self):
    obs = self.env_interface.get_observation()
    act = self.compute_action(obs)
    self.env_interface.set_agent_action(act)
```

Let's try to use a very simple example to become accustomed to these concepts.
The usual first simple environment that can be used for this kind of experiments
related to robot motion is a grid environment. A grid environment is a simplified,
discrete representation of a space where agents navigate and make decisions.
It is typically structured as a grid of cells, each representing a specific state.
Agents move between cells according to defined rules, aiming to achieve specific
goals or maximize cumulative rewards. This environment is commonly used for its
simplicity and ease of visualization, making it ideal for testing and developing
algorithms.

For simplicity, let's start considering a grid environment with only one agent.
The state of the environment is given by the state of the single cells, that
can be either free or occupied by an obstacle, and the current position of the agent.

The observation provided to the agent is the full environment state (i.e. the
state of all the cells as well as the position of the agent). The agent can
decide at each experiment step to move one single cell up or down and a single
cell left or right.
