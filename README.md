# probable-survival-1
A PS1 emulator learning project

## Build Requirements
- A compiler with C++17 support
- CMake 3.8 or newer
Additionally, there are additional dependencies on unix-like systems for [glfw](https://www.glfw.org/docs/3.3/compile_guide.html#compile_deps).

## Build Instructions
### Windows
- Open cmake-gui and point it to the project's directory.
- Define the build directory. A `build` subdirectory is recommended.
- Click on "Configure" and choose the desired compiler, click on "Finish" and then "Generate"
### Linux
Run
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
Or, alternativelly
```
cmake -B ./build -DCMAKE_BUILD_TYPE=Release
cmake --build ./build
```
