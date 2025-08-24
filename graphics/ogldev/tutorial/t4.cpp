//
// Created by fabian on 8/24/25.
//

#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "ogldev_math_3d.h"
#include "ogldev_util.h"

const char* pVSFileName = "graphics/ogldev/tutorial/shaders/shader.vert";
const char* pFSFileName = "graphics/ogldev/tutorial/shaders/shader.frag";

GLuint VBO;
GLint gScaleLocation;

void add_shader(GLuint ShaderProgram, const char* ShaderSource, GLenum ShaderType) {
    std::cout << ShaderSource << std::endl;
    GLint Lengths[1];
    Lengths[0] = (GLint)strlen(ShaderSource);
    GLuint ShaderObj = glCreateShader(ShaderType);
    glShaderSource(ShaderObj, 1, &ShaderSource, Lengths);
    glCompileShader(ShaderObj);
    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, sizeof(InfoLog), NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
    }
    glAttachShader(ShaderProgram, ShaderObj);
}

void use_shaders() {
    GLuint ShaderProgram = glCreateProgram();

    std::string vs, fs;

    if (!ReadFile(pVSFileName, vs)) {
        exit(1);
    };
    add_shader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);

    if (!ReadFile(pFSFileName, fs)) {
        exit(1);
    };

    add_shader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);


    GLint success;
    GLchar ErrorLog[1024];
    glLinkProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
    }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);

    gScaleLocation = glGetUniformLocation(ShaderProgram, "gScale");
    assert(gScaleLocation != -1 && "Uniform location not found");
}

void bind_vertices() {
    Vector3f Vertices[3];
    Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
    Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
    Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

void render_scene() {
    glClear(GL_COLOR_BUFFER_BIT); //clears the color buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
    glutPostRedisplay();
    glutSwapBuffers(); //double buffering

    //animation
    static float Scale = 0.0f;
    Scale += 0.001f;
    glUniform1f(gScaleLocation, sinf(Scale));
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    int width = 800;
    int height = 600;
    glutInitWindowSize(width, height);

    int x = 200;
    int y = 100;
    glutInitWindowPosition(x, y);
    int win = glutCreateWindow("Tutorial 04");
    printf("window id: %d\n", win);
    if (GLenum res = glewInit(); res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }
    printf("GL version: %s\n", glGetString(GL_VERSION));
    printf("GLSL version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    use_shaders();

    bind_vertices();

    glutDisplayFunc(render_scene);
    glutMainLoop();
}
