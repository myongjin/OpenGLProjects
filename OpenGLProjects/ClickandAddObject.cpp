#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Camera.h"
#include "ProjectionMatrix.h"

#include <vector>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>


using namespace std;
using namespace glm;

vec2 preMouse, currentMouse;
int windowHeight, windowWidth;

bool perspecFlag = true;
float fov = 90.f;
float n = 1.f;
float f = 10.f;


vector<vec3> balls;
camera myCamera;

void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);

    //Rotate camera
    float x_move = -30.f * (currentMouse[0] - preMouse[0]) / windowWidth;
    float y_move = -30.f * (currentMouse[1] - preMouse[1]) / windowHeight;

    myCamera.RotateCamera(myCamera.right, (float)y_move);
    myCamera.RotateCamera(myCamera.up, (float)x_move);
    preMouse = currentMouse;

    //Set projection;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (perspecFlag)
    {
        gluPerspective(fov, (float)windowWidth / (float)windowHeight, n, f);
    }
    else {
        glOrtho(-2, 2, -2, 2, n, f);
    }
    
    

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
        //myCamera.MoveCamera(myCamera.forward * scale);
        glutPostRedisplay();
        break;
    case 's':
        myCamera.RotateCamera(myCamera.forward, -1);
        //myCamera.MoveCamera(myCamera.forward * -scale);
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
    case 'v':
        perspecFlag = !perspecFlag;
        glutPostRedisplay();
        break;
    default:
        break;
    }
}

void MyTimer(int Value) {

    glutPostRedisplay();
    glutTimerFunc(40, MyTimer, 1);
}


void MyMouseClick(GLint Button, GLint State, GLint X, GLint Y) {
    if (Button == GLUT_LEFT_BUTTON && State == GLUT_DOWN) {
        preMouse = vec2(X, Y);
        currentMouse = preMouse;

        //set vec4 by mouse position in normalized coordinate (clip)
        vec4 p((float)X / (float)windowWidth * 2.0 - 1, ((float)windowHeight - (float)Y) / (float)windowHeight * 2.0 - 1, 0.f, 1.f);

        //convert it to world coordinate
        mat4 N;
        if (perspecFlag)
        {
            N = perspective(radians(fov), (float)windowWidth/(float)windowHeight, n,f);
        }
        else 
        {
            N = ortho(-2.f, 2.f, -2.f, 2.f, -0.f, 2.f);
        }
        
        mat4 V = lookAt(myCamera.eye, myCamera.at, myCamera.up);

        mat4 M = N *  V;
        mat4 invM = inverse(M);
        vec4 convP = invM * p;
        convP /= convP[3];

        vec3 finalP = convP;
        //add a ball
        balls.push_back(convP);
    }
}

void MatrixTest()
{

    mat4 rot = rotate(mat4(1), radians(45.f), vec3(0, 0, 1));
    mat4 trans = translate(mat4(1), vec3(1, 1, 1));


    mat2 A = { {1, 2}, {3, 4} };
    mat2 B = { {2, 3}, {4, 5} };
    //행렬이 곱해질때 앞뒤가 바뀌어서 곱해진다
    mat2 C = A * B;
    C = B * A;
    
}

void MyMouseMove(GLint X, GLint Y) 
{
    currentMouse = vec2(X, Y);
    glutPostRedisplay();
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
    MatrixTest();
    //balls.push_back(vec3(0, 0, 0));

    //set callback functions
    glutDisplayFunc(MyDisplay);
    
    glutTimerFunc(40, MyTimer, 1);
    glutMouseFunc(MyMouseClick);
    glutMotionFunc(MyMouseMove);
    glutReshapeFunc(MyReshape);

    //start mainloop
    glutMainLoop();
    return 0;
}
