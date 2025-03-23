# Base classes

This "journey" will go on in "stages", that are sorts of "lessons" or "single tutorials"
about some specific topic or features of the framework. Most of these stages
have an associated source code file in the `journey` subdirectory of the Ghimo
repository.

Especially in the beginning, we will proceed step by step creating the base
classes from scratch, showing their base functionalities and uses. In the following
stages we will be using the framework base classes instead, that are usually
a slightly different (or more optimized) version of what we saw previously.

## The Environment class

Let's start our journey looking at the code of the first stage of the journey:
`000b-street.py`. Let's have a look at the first lines of the file, where we
start creating the base classes:

```python linenums="6" title="000b-street.py - the Environment base class"
--8<-- "journey/000b-street.py:6:28"
```

An Environment in Ghimo is the object with which the Agent is interacting. In
the initial stages of this journey, the environments will be very simple and
are contained directly in the process that also run the agent code (see [The main loop](#the-main-loop) below).
In subsequent stages the Environment will be a full-featured simulator or even
a real robot. In the latter cases, the only thing we are interested into are
the Agent and its Interface to the Environment, while the Environment itself
is provided externally.

The Environment object maintains its own state and a set of Agents interacting
with it and is able to access their full state. It can also use a Viewer object
to show the current state.

The main functions of an Environment are listed in the above code:

- `reset()` re-initialize the Environment, resetting everything to some pre-specified
  initial state (or a random initial state)
- `step()` is the function that makes the Environment simulation to perform a step
  forward in the simulation; this usually takes the actions from the agents and
  simulates the next state
- `add_agent()` adds a new agent to the Environment, that is then aware of
  the agent and is then able to access its data to perform the simulation
- `set_viewer()` is only relevant for Environments that are internal to the
  process where also the Agents run: it allows the use of a visualizer to show
  the current state of the Environment and of the Agents running within; multiple
  viewers are possible for the same Environment, for example some based on simple
  console-based textual representations, others using complex 3D visualization
  techniques; please note that if you want to create a new Environment, you have
  to call the Viewer's `render()` function before updating the Environment
  internal state.

## The main loop

After we create the classes inheriting from Ghimo's basic classes, the main
loop becomes very simple:

```py linenums="126" title="000b-street.py - the main loop"
--8<-- "journey/000b-street.py:126:"
```

It is important to notice that Agent also has a `step()` function, where it
reads the current observation from the environment and decides the next action
to perform. It's the Environment's `step()` function that makes the state
advance, the Agent only decide "the next action to perform". This is the main
reason why the visualization of the environment should be the first function
to be called in the Environment's `step()` function: it needs to show the state
of the environment (and possibly the information used and provided by the agents
to take their decisions) *before* the state changes.

## The Agent class

```py linenums="30" title="000b-street.py - the Agent base class"
--8<-- "journey/000b-street.py:30:38"
```

The skeleton Agent class is very simple, it just has a `step()` function, where
it uses the Interface's methods to retrieve the current observation and decide
what is the next action to perform. The Agent's `interface` is set by the
Interface methods (see below).


## The Viewer class

```py linenums="40" title="000b-street.py - the Agent base class"
--8<-- "journey/000b-street.py:40:49"
```

## The Environment <-> Agent Interface


```py linenums="51" title="000b-street.py - the Agent base class"
--8<-- "journey/000b-street.py:51:66"
```
