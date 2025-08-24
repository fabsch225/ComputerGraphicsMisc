cmake_minimum_required(VERSION 3.29)
project(ComputerGraphik1)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

add_executable(test0 graphics/example1.cpp)

add_executable(tesseract graphics/tesseract.cpp)

add_executable(minecraft graphics/minecraft.cpp)

add_executable(test1 graphics/polygon_dino.cpp)

add_executable(test2 graphics/voxels.cpp)

include_directories(${OPENGL_INCLUDE_DIRS})

target_link_libraries(test2
        ${OPENGL_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${GLUT_LIBRARIES}
)

target_link_libraries(test1 ${OPENGL_LIBRARIES} glut)
target_link_libraries(test0 ${OPENGL_LIBRARIES} glut)
target_link_libraries(tesseract ${OPENGL_LIBRARIES} glut)
target_link_libraries(minecraft ${OPENGL_LIBRARIES} glut)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
add_executable(barnes-hut graphics/n-bodies/main.cpp
        graphics/n-bodies/quad.h
        graphics/n-bodies/barnes_hut_tree.h)

target_link_libraries(barnes-hut ${SDL2_LIBRARIES})

add_executable(double-pendulum graphics/pendulum/double_pendulum.cpp)
target_link_libraries(double-pendulum ${SDL2_LIBRARIES})

# ogldev (OpenGL Tutorial)

function(add_ogldev_tutorial tutorial_name)
    add_executable(ogldev_tutorial${tutorial_name}
            graphics/ogldev/Common/ogldev_util.cpp
            graphics/ogldev/tutorial/t${tutorial_name}.cpp)
    target_include_directories(ogldev_tutorial${tutorial_name} PRIVATE graphics/ogldev/include)
    target_link_libraries(ogldev_tutorial${tutorial_name}
            ${OPENGL_LIBRARIES}
            ${GLEW_LIBRARIES}
            ${GLUT_LIBRARIES}
    )
    file(GLOB tutorial_files
            ${CMAKE_CURRENT_SOURCE_DIR}/graphics/ogldev/tutorial/shaders/*
    )
    file(COPY ${tutorial_files}
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/graphics/ogldev/tutorial/shaders)
endfunction()

add_ogldev_tutorial(2)
add_ogldev_tutorial(4)
