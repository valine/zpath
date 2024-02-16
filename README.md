# ZPath
Interactive, hardware accelerated computation graphs
<img width="1108" alt="Screenshot 2024-02-15 at 6 03 14 PM" src="https://github.com/valine/zpath/assets/14074844/f06243d0-793e-4b2d-a6e7-6830f0390d4f">


## Build instructions

Dependencies

* CMake https://cmake.org/download/
* GLFW git@github.com:glfw/glfw.git 
* GLM https://github.com/g-truc/glm/archive/0.9.8.5.tar.gz 
* Freetype https://www.freetype.org/ 
* Assimp git@github.com:assimp/assimp.git
* Giac https://www-fourier.ujf-grenoble.fr/~parisse/giac.html
* libpng http://www.libpng.org/pub/png/libpng.html
* zlib https://zlib.net/

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

### To build on MacOS
Install CMake and XCode

From Terminal
cd zpath
mkdir build && cd build 
camke -DCMAKE_INSTALL_PREFIX=/Applications ..
make


### UI Examples
```
// Button example
auto* button = new ZButton("Button name", parent);
saveButton->setOnClick([this](ZView* sender) {
    // Handle click
});

// Slider example
auto* slider = new ZSlider("Slider name", min, max, defaultValue, parent);
slider->setOnSlide([this](ZView* sender, float value, bool wasUserAction) {
    // Handle slide
});

// Checkbox example
auto* checkbox = new ZCheckbox("Checkbox name", parent);
checkbox->setOnClick([this](ZView* sender, bool checked) {
    // Handle check change
});
```
