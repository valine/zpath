# ZPath
Render engine and cross platform UI framework 

## Build instructions

Dependencies

* CMake https://cmake.org/download/
* GLFW git@github.com:glfw/glfw.git // downloaded automatically
* GLM https://github.com/g-truc/glm/archive/0.9.8.5.tar.gz // downloaded automatically
* Freetype https://www.freetype.org/ // sudo apt-get install libfreetype6-dev
* Assimp git@github.com:assimp/assimp.git // downloaded automatically

Other packages for linux

freeglut3-dev
libglu1-mesa-dev
libglu1-xorg-dev

### To build

```bash
cd zpath 
mkdir build && cd build
cmake ..
make
```
