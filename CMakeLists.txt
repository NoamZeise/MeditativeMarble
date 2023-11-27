cmake_minimum_required(VERSION 3.14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

project(bmjam VERSION 1.0)

add_subdirectory(Graphics-Environment)
add_subdirectory(src)
