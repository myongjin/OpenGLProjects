//마우스로 사각형 그리기
#include <GL/glew.h>
#include <gl/freeglut.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>

#include <vector>
#include <glm/glm.hpp>

GLint TopLeftX, TopLeftY, BottomRightX, BottomRightY;

void MyDisplay() {
    glViewport(0, 0, 300, 300);
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_POLYGON);
    glVertex3f(TopLeftX / 300.0 * 2 - 1, (300 - TopLeftY) / 300.0 * 2 - 1, 0.0);
    glVertex3f(TopLeftX / 300.0 * 2 - 1, (300 - BottomRightY) / 300.0 * 2 - 1, 0.0);
    glVertex3f(BottomRightX / 300.0 * 2 - 1, (300 - BottomRightY) / 300.0 * 2 - 1, 0.0);
    glVertex3f(BottomRightX / 300.0 * 2 - 1, (300 - TopLeftY) / 300.0 * 2 - 1, 0.0);
    glEnd();
    glFlush();
}

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) {
    if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
        TopLeftX = X;
        TopLeftY = Y;
    }
}

void MyMouseMove(GLint X, GLint Y) {
    BottomRightX = X;
    BottomRightY = Y;
    glutPostRedisplay();
}

void MyMouseMove2(GLint X, GLint Y) {
    BottomRightX = Y;
    BottomRightY = X;
    glutPostRedisplay();
}

void MyKeyboard(unsigned char KeyPressed, int X, int Y) {
    switch (KeyPressed) {
    case 'Q':
        exit(0); break;
    case 'q':
        exit(0); break;
    case 27:                //'esc' 키의 아스키 코드 값
        exit(0); break;
    }
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(300, 300);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Drawing Example");

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glutDisplayFunc(MyDisplay);
    glutMouseFunc(MyMouseClick);
    glutMotionFunc(MyMouseMove2);
    glutKeyboardFunc(MyKeyboard);
    glutMainLoop();
    return 0;
}
