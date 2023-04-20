# PlanetarySystemViewer

Launch an interactive, 3d model of a planetary system

![Alt text](/images/init.gif?raw=true)

## How to build

*Windows*

Open git bash to the directory containing this repository.

```
mesh-viewer $ mkdir build
mesh-viewer $ cd build
mesh-viewer/build $ cmake ..
mesh-viewer/build $ start viewer.sln
```

Run from the git bash command shell, 

```
mesh-viewer/build $ ../bin/Debug/viewer.exe
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
PlanetarySystemViewer/build $ ../bin/viewer
```

## Features

Background implemented with cubemap

Unique textures and meshes

Launch a viewer with orbiting planets

Rotate and zoom abilities

Click on different planets

Phong shading

Area lighting

Particle system tracks orbits

## Images
