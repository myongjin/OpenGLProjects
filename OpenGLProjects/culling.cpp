#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Camera.h"

#include <vector>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;


camera myCamera;

void Problem6()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set culling face
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    //Set projection;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-5, 5, -5, 5, 0, 10);

    //Set camera
    vec3 up(0, 1, 0);
    vec3 eye(0, 0, 5);
    vec3 at(0, 0, 0);

    //Set matrix mode
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);


    vec3 a(1, 3, 2);
    vec3 b(2, -4, 0);
    vec3 c(-4, 0, -2);

    vec3 ba = b - a;
    vec3 ca = c - a;

    //ccw (+)
    vec3 result = cross(ba, ca);
    //cw (-)
    //result = cross(ca, ba);

    glColor3f(1, 0, 0);
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_LINE);

    glBegin(GL_POLYGON);
    glVertex3f(a[0], a[1], a[2]);
    glVertex3f(b[0], b[1], b[2]);
    glVertex3f(c[0], c[1], c[2]);
    glEnd();

    glFlush();
}


int main(int argc, char** argv) {
    //Create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample Drawing");

    //Clear and set background color;
    glClearColor(1.0, 1.0, 1.0, 1.0);

    //Init camera
    vec3 center(0, 0, 2);
    vec3 at(0, 0, 0);
    vec3 up(0, 1, 0);

    myCamera.InitCamera(center, at, up);

    //set callback functions
    glutDisplayFunc(Problem6);

    //start mainloop
    glutMainLoop();
    return 0;
}