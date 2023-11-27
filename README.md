# Bigmode Game Jam 2023
Two Week Game jam ([itch page](https://itch.io/jam/bigmode-2023)).
Theme: *Mode*


# Building

## Requirements:

* C++ Compiler (tested with GCC and MSVC)
* CMake 
* Linux - libsndfile, libportaudio
* Windows - sndfile, portaudio builds

## Setup:

Clone this repo with submodules


## Building:

In this project's root do
```
mkdir build && cd build
cmake ..
cmake --build . --parallel
```
