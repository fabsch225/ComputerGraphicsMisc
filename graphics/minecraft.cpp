//
// Created by fabian on 2/21/25.
//

#include <cmath>
#include <GL/glut.h>
#include <vector>

float camX = 0.0f, camY = 2.0f, camZ = 5.0f;
float camYaw = 0.0f, camPitch = 0.0f;

int lastX = 0, lastY = 0;
bool mouseDown = false;

struct Block {
    float x, y, z;
    bool active;
};

std::vector<Block> blocks;

void drawCube(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);

    glBegin(GL_QUADS);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);

    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);

    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);

    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);

    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);

    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glEnd();

    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glRotatef(camPitch, 1.0f, 0.0f, 0.0f);
    glRotatef(camYaw, 0.0f, 1.0f, 0.0f);
    glTranslatef(-camX, -camY, -camZ);

    glBegin(GL_QUADS);
    glColor3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-10.0f, 0.0f, -10.0f);
    glVertex3f(-10.0f, 0.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, 10.0f);
    glVertex3f(10.0f, 0.0f, -10.0f);
    glEnd();

    for (const Block& block : blocks) {
        if (block.active) {
            drawCube(block.x, block.y, block.z);
        }
    }

    glutSwapBuffers();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)width/height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    float speed = 0.1f;
    switch (key) {
        case 'w':
            camX += sin(camYaw * 3.14159f / 180.0f) * speed;
            camZ -= cos(camYaw * 3.14159f / 180.0f) * speed;
            break;
        case 's':
            camX -= sin(camYaw * 3.14159f / 180.0f) * speed;
            camZ += cos(camYaw * 3.14159f / 180.0f) * speed;
            break;
        case 'a':
            camX -= cos(camYaw * 3.14159f / 180.0f) * speed;
            camZ -= sin(camYaw * 3.14159f / 180.0f) * speed;
            break;
        case 'd':
            camX += cos(camYaw * 3.14159f / 180.0f) * speed;
            camZ += sin(camYaw * 3.14159f / 180.0f) * speed;
            break;
        case ' ':
            camY += speed;
            break;
        case 'c':
            camY -= speed;
            break;
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            mouseDown = true;
            lastX = x;
            lastY = y;
        } else {
            mouseDown = false;
        }
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        Block newBlock;
        newBlock.x = round(camX);
        newBlock.y = round(camY) - 1;
        newBlock.z = round(camZ);
        newBlock.active = true;
        blocks.push_back(newBlock);
    }
}

void motion(int x, int y) {
    if (mouseDown) {
        int dx = x - lastX;
        int dy = y - lastY;

        camYaw += dx * 0.2f;
        camPitch += dy * 0.2f;

        if (camPitch > 89.0f) camPitch = 89.0f;
        if (camPitch < -89.0f) camPitch = -89.0f;

        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Minecraft Clone");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}