/***********************************************
    Solution A3, Computergraphics I, SS 2001
    Author: Susanne Kroemker, IWR - UNIVERSITAET HEIDELBERG
    phone:
    +49 (0)6221 54 8883
    email:
    kroemker@iwr.uni-heidelberg.de
************************************************/

#include <cstring>
    #include <GL/glut.h>

void winPrint (char *);
void winPrintPos (int, int, int, char *);
void display(void);
int main(int, char **);
void winPrintPos (int x, int y, char *str)
{
    glRasterPos2i(x,y);
    for (int j=0; j<strlen(str); j++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,str[j]);
}
void display(void)
{
    char *words = "Hello World";
    glClear (GL_COLOR_BUFFER_BIT);
    glColor3f (1.0,0.0,0.0);
    glBegin(GL_POLYGON);
    glVertex2i (0, 20);
    glVertex2i (90, 70);
    glVertex2i (100, 85);
    glVertex2i (90, 100);
    glVertex2i (55, 105);
    glVertex2i (25, 100);
    glVertex2i (0, 80);
    glEnd();
    glBegin(GL_POLYGON);
    glVertex2i (0, 20);
    glVertex2i (-90, 70);
    glVertex2i (-100, 85);
    glVertex2i (-90, 100);
    glVertex2i (-55, 105);
    glVertex2i (-25, 100);
    glVertex2i (0, 80);
    glEnd();
    glColor3f (0.7,0.7,1.0);
    winPrintPos(-20,70,words);
    glFlush();
}
int main(int argc,char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE |GLUT_RGB);
    glutInitWindowSize (250,250);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("Hello World");
    glClearColor (1.0, 1.0, 1.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-120.0, 120.0, 15.0, 120.0);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}