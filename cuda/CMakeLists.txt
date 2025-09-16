cmake_minimum_required(VERSION 4.0)

project(cuda-misc LANGUAGES CUDA CXX)

find_package(CUDAToolkit REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

list(APPEND CMAKE_BUILD_RPATH "${CUDAToolkit_LIBRARY_DIR}" "${cuDNN_LIBRARY_DIR}")

add_subdirectory(mandelbrot)