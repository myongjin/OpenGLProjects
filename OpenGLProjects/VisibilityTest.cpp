#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>


bool cw = false;
bool culling = false;
bool hidden = false;
bool backculling = true;
bool visibility_test = false;

void InitLight() {
    GLfloat mat_diffuse[]    = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_ambient[]    = { 0.5, 0.4, 0.3, 1.0 };
    GLfloat mat_shininess[]  = { 50.0 };
    GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat light_diffuse[]  = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat light_ambient[]  = { 0.3, 0.3, 0.3, 1.0 };
    GLfloat light_position[] = { -3, 2, 3.0, 0.0 };

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void InitVisibility() {

    if (culling) {
        glEnable(GL_CULL_FACE);
    }
    else {
        glDisable(GL_CULL_FACE);
    }

    if (cw) {
        glFrontFace(GL_CW);
    }
    else {
        glFrontFace(GL_CCW);
    }
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    if (backculling) {
        glCullFace(GL_BACK);
    }
    else {
        glCullFace(GL_FRONT);
    }

    if (hidden) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
}

void MyKeyboard(unsigned char KeyPressed, int X, int Y)
{
    switch (KeyPressed) {
    
    case '1':
        cw = !cw;
        break;
    case '2':
        culling = !culling;
        break;
    case '3':
        backculling = !backculling;
        break;
    case '4':
        hidden = !hidden;
        break;
    case '5':
        visibility_test = !visibility_test;
        break;
    }

    glutPostRedisplay();
}

void MyDisplay() {
    if (visibility_test) {
        InitVisibility();
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 1, 1.3, 0.0, -0.5, -1.0, 0.0, 1.0, 0.0);
    glutSolidTeapot(0.58);
    glFlush();
}

void MyReshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 10.0);
    //gluPerspective(60, 1, 0.01, 50);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample Drawing");
    glClearColor(0.5, 0.5, 0.5, 0.0);
    InitLight();

    
    glutKeyboardFunc(MyKeyboard);
    glutDisplayFunc(MyDisplay);
    glutReshapeFunc(MyReshape);
    glutMainLoop();
    return 0;
}
