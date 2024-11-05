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

static int Day = 0, Time = 0;

int windowHeight, windowWidth;


camera myCamera;
void DrawSolarSystem() {

    //get camera variables from camera class
    vec3 eye = myCamera.eye;
    vec3 at = myCamera.at;
    vec3 up = myCamera.up;


    glMatrixMode(GL_MODELVIEW); //set matrix mode
    glLoadIdentity(); //set Identity 
    //set view transform matrix
    gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);


    //draw solar system
    glColor3f(1.0, 0.3, 0.3);
    glutSolidSphere(0.2, 20, 16);

    glPushMatrix();
    glRotatef((GLfloat)Day, 0.0, 1.0, 0.0);
    glTranslatef(0.7, 0.0, 0.0);
    glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);
    glColor3f(0.5, 0.6, 0.7);
    glutSolidSphere(0.1, 10, 8);
    glPushMatrix();
    glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);
    glTranslatef(0.2, 0.0, 0.0);
    glColor3f(0.9, 0.8, 0.2);
    glutSolidSphere(0.04, 10, 8);
    glPopMatrix();
    glPopMatrix();

}

void MyDisplay() {
//
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set projection;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20, (float)windowWidth / (float)windowHeight, 1.f, 10.f);

    
    //Draw Scene
    DrawSolarSystem();

    
    //Send it to buffer
    glFlush();

}


void MyReshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;

    //Set viewport
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}


void MyTimer(int Value) {
    Day = (Day + 2) % 360;
    Time = (Time + 1) % 360;

    mat4 rot = rotate(mat4(1.f), radians(1.f), vec3(0,0,1));

    vec3 rotatedDirec = rot * vec4((myCamera.eye - myCamera.at),0);
    vec3 rotatedUp = rot * vec4(myCamera.up, 0);
    myCamera.eye = myCamera.at + rotatedDirec;
    myCamera.up = rotatedUp;
    myCamera.UpdateCamera();
    glutPostRedisplay();
    glutTimerFunc(40, MyTimer, 1);
}



int main(int argc, char** argv) {
    //Create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample Drawing");

    //Clear and set background color;
    glClearColor(0.0, 0.0, 0.0, 1.0);

    //get window variables
    windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    windowHeight = glutGet(GLUT_WINDOW_HEIGHT);



    //Init camera
    vec3 center(0, 2, 4);
    vec3 at(0.2, 0, 0);
    vec3 up = cross(vec3(1, 0, 0), at - center);
    up = normalize(up);
    myCamera.InitCamera(center, at, up);

    //set callback functions
    glutDisplayFunc(MyDisplay);
    glutTimerFunc(40, MyTimer, 1);
    glutReshapeFunc(MyReshape);

    //start mainloop
    glutMainLoop();
    return 0;
}
