# Installing and setting up the environment

In order to start playing with the steps of the journey, you have to install
`ghimo`. The first thing to do is to download the repository:

```bash
$ git checkout https://github.com/madmage/ghimo
```

Now we have to compile it:

```bash
$ cd ghimo
$ mkdir build
$ cd build
$ cmake ..
$ make
$ make install
```

This will first (`make`) compile `ghimo` in the `build` directory, and then
install it in the `install` directory.

The last thing to do before starting with the next step is to set up the
environment:

```bash
$ cd ..
$ . setup.bash
```

this latter step should be done every time you open a new shell (e.g., if
you switch off your computer).
