cmake_minimum_required(VERSION 3.29)
project(ComputerGraphik1)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

add_executable(test0
        #graphics/example1.cpp
        #graphics/tesseract.cpp
        graphics/minecraft.cpp)

add_executable(test1
        graphics/polygon_dino.cpp)

add_executable(test2 graphics/voxels.cpp)

include_directories(${OPENGL_INCLUDE_DIRS})

target_link_libraries(test2
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${GLUT_LIBRARIES}
)

target_link_libraries(test1 ${OPENGL_LIBRARIES} glut)
target_link_libraries(test0 ${OPENGL_LIBRARIES} glut)