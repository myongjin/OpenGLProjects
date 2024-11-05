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

vec2 preMouse, currentMouse;

int windowHeight, windowWidth;

camera myCamera;

void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) {
    if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
        preMouse = vec2(X, Y);
        currentMouse = preMouse;
    }
}

void MyMouseMove(GLint X, GLint Y) {
    currentMouse = vec2(X, Y);
    glutPostRedisplay();
}

void MyReshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;

    //Set viewport
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void DrawCoordinate()
{
    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);

    glEnd();
}


void DrawSquare(vec3 center, float w, float h)
{
    glBegin(GL_LINES);
    vec3 v;
    v = center + vec3(-w / 2, -h / 2, 0);
    glVertex3f(v[0],v[1],v[2]);
    v = center + vec3(-w / 2, h / 2, 0);
    glVertex3f(v[0], v[1], v[2]);

    glVertex3f(v[0], v[1], v[2]);
    v = center + vec3(w / 2, h / 2, 0);
    glVertex3f(v[0], v[1], v[2]);

    glVertex3f(v[0], v[1], v[2]);
    v = center + vec3(w / 2, -h / 2, 0);
    glVertex3f(v[0], v[1], v[2]);

    glVertex3f(v[0], v[1], v[2]);
    v = center + vec3(-w / 2, -h / 2, 0);
    glVertex3f(v[0], v[1], v[2]);

    glEnd();
}


void DrawCircle(vec3 center, float radius, int res)
{
    glBegin(GL_LINES);
    vec3 p(radius, 0, 0);
    mat4 rot = rotate(mat4(1.f), radians(360.f / (float)res), vec3(0, 0, 1));
    vec3 v1, v2;
    v1 = center + p;
    for (int i = 0; i < res; i++)
    {
        p = rot * vec4(p, 0);
        v2 = center + p;
        glVertex3f(v1[0], v1[1], v1[2]);
        glVertex3f(v2[0], v2[1], v2[2]);
        v1 = v2;
    }
    glEnd();
}
void DrawModelA() {
    DrawCoordinate();
    glColor3f(0, 0, 0);
    DrawSquare(vec3(0, 0, 0), 8, 4);
}

void DrawModelB() {
    DrawCoordinate();
    glColor3f(0, 0, 0);
    DrawCircle(vec3(0, 0, 0), 2, 30);
}

void Problem1() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set projection;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-20, 20, -20, 20, 0, 5);

    //set camera
    vec3 up(0, 1, 0);
    vec3 eye(0, 0, 1);
    vec3 at(0, 0, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);

    //Draw coordinate before transforming the orginal coordinate
    DrawCoordinate();
    //Draw the cooridnate after traslating it
    glTranslatef(3, 1, 0);
    DrawCoordinate();

    glRotatef(60, 0, 0, 1);
    DrawCoordinate();

    glTranslatef(-1, 0, 0);
    DrawCoordinate();
    
    //Draw a point
    glPointSize(5);
    glBegin(GL_POINTS);
        glVertex3f(1, 1, 0);
    glEnd();
}

void Problem3()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //Set projection;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-20, 20, -20, 20, 0, 5);

    //set camera
    vec3 up(0, 1, 0);
    vec3 eye(0, 0, 1);
    vec3 at(0, 0, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);
    
    ////3-1
    //glPushMatrix();
    //    glTranslatef(2, 5, 0);
    //    glRotatef(90, 0, 0, 1);
    //    DrawModelA();
    //glPopMatrix();

    ////3-2
    //glPushMatrix();
    //    glTranslatef(3, 3, 0);
    //    glRotatef(-30, 0, 0, 1);
    //    glScalef(2, 1, 1);
    //    DrawModelB();
    //glPopMatrix();

    //3-3
    glPushMatrix();
        glTranslatef(5, 7, 0);
        glScalef(10.f/8.f, 1, 1);
        DrawModelA();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(7, 1, 0);
        glRotatef(-30, 0, 0, 1);
        DrawModelA();
    glPopMatrix();
    
    //3-4
    /*glPushMatrix();
        glTranslatef(3, 6, 0);
        glRotatef(-45, 0, 0, 1);
        DrawModelB();
        glPushMatrix();
            glTranslatef(6, 0, 0);
            DrawModelA();
        glPopMatrix();

        glPushMatrix();
            glRotatef(90, 0, 0, 1);
            glTranslatef(6, 0, 0);
            glScalef(1, 0.5, 1);
            DrawModelA();
        glPopMatrix();
    glPopMatrix();*/

    glFlush();
}

void Problem4()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //Set projection;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, 2, 1, 6);

    //set camera
    vec3 up(0, 1, 0);
    vec3 eye(1, 0, 2);
    vec3 at(1, 0, 0);

    vec4 p(2, 1, 0, 1);
    mat4 N = perspective(radians(90.f), 2.f, 1.f, 6.f);
    mat4 V = lookAt(eye, at, up);

    vec4 pvc = V * p;
    vec4 result = N*V*p;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);

    glColor3f(1, 0, 0);
    glPointSize(10);
    glBegin(GL_POINTS);
    glVertex3f(2, 1, 0);
    glEnd();


    glFlush();
}

void Problem6() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set culling face
    glEnable(GL_CULL_FACE);
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
    result = cross(ca, ba);

    

    glColor3f(1, 0, 0);
    glBegin(GL_TRIANGLES);
        glVertex3f(a[0], a[1], a[2]);
        
        glVertex3f(b[0], b[1], b[2]);
        glVertex3f(c[0], c[1], c[2]);
        
    glEnd();

    glFlush();
}
void MyDisplay() {
    //
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    //Rotate camera
    float x_move = -30.f * (currentMouse[0] - preMouse[0]) / windowWidth;
    float y_move = -30.f * (currentMouse[1] - preMouse[1]) / windowHeight;

    //myCamera.RotateCamera(myCamera.right, (float)y_move);
    //myCamera.RotateCamera(myCamera.up, (float)x_move);
    preMouse = currentMouse;

    //Set projection;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(20, (float)windowWidth / (float)windowHeight, 1.f, 10.f);
    glOrtho(-5, 5, -5, 5, 0, 5);

    //Draw Scene
    Problem1();


    //Send it to buffer
    glFlush();
    //glDisable(GL_DEPTH_TEST);

}


int main(int argc, char** argv) {
    //Create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample Drawing");

    //Clear and set background color;
    glClearColor(1.0, 1.0, 1.0, 1.0);

    //get window variables
    windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    windowHeight = glutGet(GLUT_WINDOW_HEIGHT);



    //Init camera
    vec3 center(0, 0, 2);
    vec3 at(0, 0, 0);
    vec3 up(0,1,0);

    myCamera.InitCamera(center, at, up);

    glEnable(GL_DEPTH_TEST);

    //set callback functions
    glutDisplayFunc(Problem3); 
    //glutKeyboardFunc(MyKeyboard);
   // glutTimerFunc(40, MyTimer, 1);
    glutMouseFunc(MyMouseClick);
    glutMotionFunc(MyMouseMove);
    glutReshapeFunc(MyReshape);

    //start mainloop
    glutMainLoop();
    return 0;
}
