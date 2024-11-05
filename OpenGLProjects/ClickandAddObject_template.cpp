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

int windowHeight, windowWidth;

bool perspecFlag = false;
float fov = 45.f;
float n = 0.f;
float f = 10.f;



vector<vec3> balls;

camera myCamera;

void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);


    //Set projection;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.f, 2.f, -2.f, 2.f, 0.f, 10.f);

    //Draw Scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //set Identity

    //get camera variables from camera class
    vec3 eye = myCamera.eye;
    vec3 at = myCamera.at;
    vec3 up = myCamera.up;
    gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);

    //draw balls
    for (int i = 0; i < balls.size(); i++) {
        glPushMatrix();
        glTranslatef(balls[i][0], balls[i][1], balls[i][2]);
        glutWireSphere(0.2, 20, 16);
        glPopMatrix();
    }

    //Send it to buffer
    glFlush();
}


void MyReshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;

    //Set viewport
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void MyKeyboard(unsigned char key, int x, int y) {
    float scale = 0.1;
    switch (key) {
    case 'w':
        myCamera.RotateCamera(myCamera.up, 1);
        glutPostRedisplay();
        break;
    case 's':
        myCamera.RotateCamera(myCamera.forward, -1);
        glutPostRedisplay();
        break;
    case 'a':
        myCamera.MoveCamera(myCamera.right * -scale);
        glutPostRedisplay();
        break;
    case 'd':
        myCamera.MoveCamera(myCamera.right * scale);
        glutPostRedisplay();
        break;
    default:
        break;
    }
}


void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) {
    if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {

        //set vec4 from x and y in normalized coordinate (clip)


        //convert it to view coordinate  

    }
}



int main(int argc, char** argv) {
    //Create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample Drawing");

    //Clear and set background color;
    glClearColor(0.0, 0.0, 0.0, 1.0);

    //get window size
    windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    windowHeight = glutGet(GLUT_WINDOW_HEIGHT);


    //Init camera
    vec3 eye(2, 2, 0);
    vec3 at(0, 0, 0);
    vec3 up = cross(vec3(1, 0, 0), at - eye);
    up = normalize(up);
    myCamera.InitCamera(eye, at, up);


    //debug
    balls.push_back(vec3(0, 0, 0));

    //set callback functions
    glutDisplayFunc(MyDisplay);
    glutKeyboardFunc(MyKeyboard);
    glutMouseFunc(MyMouseClick);
    glutReshapeFunc(MyReshape);

    //start mainloop
    glutMainLoop();
    return 0;
}
