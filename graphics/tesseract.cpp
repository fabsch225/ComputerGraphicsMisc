//
// Created by fabian on 2/21/25.
//

#include <GL/glut.h>
#include <cmath>
#include <vector>

struct Vec4 {
    float x, y, z, w;
    Vec4(float x_ = 0, float y_ = 0, float z_ = 0, float w_ = 0)
        : x(x_), y(y_), z(z_), w(w_) {}
};

float angle = 0.0f;
std::vector<Vec4> vertices;
std::vector<std::pair<int, int>> edges;

void rotate4D(Vec4& v, float theta, bool xyPlane) {
    float tempX = v.x;
    float tempY = v.y;
    float tempZ = v.z;
    float tempW = v.w;

    if (xyPlane) {
        v.x = tempX * cos(theta) - tempY * sin(theta);
        v.y = tempX * sin(theta) + tempY * cos(theta);
    } else {
        v.z = tempZ * cos(theta) - tempW * sin(theta);
        v.w = tempZ * sin(theta) + tempW * cos(theta);
    }
}

Vec4 project4Dto3D(const Vec4& v) {
    float distance = 2.0f;
    float factor = distance / (distance - v.w);
    return Vec4(v.x * factor, v.y * factor, v.z * factor, 0);
}

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);

    for (int i = 0; i < 16; i++) {
        float x = (i & 1) ? 1.0f : -1.0f;
        float y = (i & 2) ? 1.0f : -1.0f;
        float z = (i & 4) ? 1.0f : -1.0f;
        float w = (i & 8) ? 1.0f : -1.0f;
        vertices.push_back(Vec4(x, y, z, w));
    }

    for (int i = 0; i < 16; i++) {
        for (int j = i + 1; j < 16; j++) {
            int diff = i ^ j;
            if (diff == 1 || diff == 2 || diff == 4 || diff == 8) {
                edges.push_back({i, j});
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    //rotation
    std::vector<Vec4> rotatedVertices = vertices;
    for (auto& v : rotatedVertices) {
        rotate4D(v, angle, true);
        rotate4D(v, angle * 0.7f, false);
    }

    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    for (const auto& edge : edges) {
        Vec4 v1 = project4Dto3D(rotatedVertices[edge.first]);
        Vec4 v2 = project4Dto3D(rotatedVertices[edge.second]);
        glVertex3f(v1.x, v1.y, v1.z);
        glVertex3f(v2.x, v2.y, v2.z);
    }
    glEnd();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, (float)w/h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void idle() {
    angle += 0.02f;
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Tesseract");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}