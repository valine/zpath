# ZPath
Physically based render engine and cross platform UI framework 

## Build instructions

Dependencies

* CMake https://cmake.org/download/
* GLFW git@github.com:glfw/glfw.git 
* GLM https://github.com/g-truc/glm/archive/0.9.8.5.tar.gz 
* Freetype https://www.freetype.org/ 
* Assimp git@github.com:assimp/assimp.git

### To build on Linux

Packages to install on ubuntu

* libfreetype6-dev
* libglu1-mesa-dev

```bash
cd zpath 
mkdir build && cd build
cmake ..
make
```


### To build on Windows

Install Visual Studio build tools

From the Developer command prompt run 
```bash
cd zpath 
mkdir build && cd build
cmake ..
msbuild ALL_BUILD.vsfx
```

### UI Examples
```
// Button example
auto* button = new ZButton("Button name", parent);
saveButton->setOnClick([this](ZView*){
    // Do stuff here
});

// Slider example
auto* slider = new ZSlider("Slider name", min, max, defaultValue, parent);
slider->setOnSlide([this](ZView*, float v){
    // Do stuff
});

// Checkbox example
auto* checkbox = new ZCheckbox("Checkbox name", getResourcePath(), parent);
checkbox->setOnClick([this](ZView*, bool checked){
    // Do stuff
});
```
