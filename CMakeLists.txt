cmake_minimum_required(VERSION 3.14)

include("Graphics-Environment/cmake/project-setup.cmake")

project(bmjam VERSION 1.0)

if(BUILD_STATIC)
  set(GRAPHICS_BUILD_STATIC ON)
endif()
add_subdirectory(Graphics-Environment)
add_subdirectory(src)
