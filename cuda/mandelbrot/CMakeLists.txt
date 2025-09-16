add_executable(mandelbrot main.cu)


set_target_properties(mandelbrot PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON
        CUDA_ARCHITECTURES 75
)

target_link_libraries(mandelbrot
        PRIVATE
        CUDA::cudart
        OpenGL::GL
        OpenGL::GLU
        GLUT::GLUT
)
