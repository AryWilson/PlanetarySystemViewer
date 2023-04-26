# PlanetarySystemViewer

Launch an interactive, 3d model of a planetary system
![Alt text](/images/rep.gif?raw=true)


## How to build

*Windows*

Open git bash to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake ..
mesh-viewer/build $ start Viewer.sln
```

Run from the git bash command shell, 

```
mesh-viewer/build $ ../bin/Debug/Viewer.exe
```

*macOS*

Open terminal to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake ..
mesh-viewer/build $ make
```

To run each program from build, you would type

```
PlanetarySystemViewer/build $ ../bin/Viewer
```

## Features

Background implemented with a unique cubemap


Launch a viewer with orbiting planets
![Alt text](/images/init.gif?raw=true)


Rotate and zoom abilities
![Alt text](/images/movement.gif?raw=true)


Unique textures, meshes, and normal maps
![Alt text](/images/unique.gif?raw=true)


Click on a planet to view it in more detail and return to the planetary model by clicking the space bar
![Alt text](/images/click.gif?raw=true)

Phong shading in the main system and two unique shaders in the individual models
Shaders for gas planets
![Alt text](/images/vertextMove.gif?raw=true)
Shaders for rocky planets
![Alt text](/images/click.gif?raw=true)


Particle system tracks orbits
![Alt text](/images/particles.gif?raw=true)

Randomize the planetary system by clicking r
Return to the default planetary system by clicking n
![Alt text](/images/randomize.gif?raw=true)


## Images
