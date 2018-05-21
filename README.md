# ZPath
Render engine and cross platform UI framework 

## Build instructions

Dependencies

* CMake https://cmake.org/download/
* GLFW git@github.com:glfw/glfw.git 
* GLM https://github.com/g-truc/glm/archive/0.9.8.5.tar.gz 
* Freetype https://www.freetype.org/ 
* Assimp git@github.com:assimp/assimp.git

Packages to install on ubuntu

* libfreetype6-dev
* freeglut3-dev
* libglu1-mesa-dev
* libglu1-xorg-dev

### To build

```bash
cd zpath 
mkdir build && cd build
cmake ..
make
```
