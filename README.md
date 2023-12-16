# Meditative Marble

Made for a two week game jam: [Bigmode 2023](https://itch.io/jam/bigmode-2023).
Theme: *Mode*

Made with my [rendering library](https://github.com/NoamZeise/Graphics-Environment).

# Building

## Requirements:

* C++ Compiler (tested with GCC and MSVC)
* CMake 
* [Vulkan Headers](https://vulkan.lunarg.com/)

## Setup:

Clone this repo with submodules
```
git clone https://github.com/NoamZeise/MeditativeMarble.git --recurse-submodules
```

## Building:

In this project's root do
```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DNO_AUDIO=true
cmake --build . --parallel
```

After, the binary should be at `build/src/bmjam23`.

## Troubleshooting:

* If you get missing <vulkan/...> errors 
pass `-D VULKAN_HEADERS_INSTALL_DIR=/your/path/to/your/installed/vulkan/headers`
to cmake when generating this project
