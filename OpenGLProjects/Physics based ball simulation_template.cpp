#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Camera.h"

#include <vector>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

//sound
#pragma comment(lib, "winmm")
#include <mmsystem.h>

using namespace std;
using namespace glm;

vec2 preMouse, currentMouse;
int windowHeight, windowWidth;
bool mouseMoving = false;
bool animate = false;

camera myCamera;

struct Ball {
    vec3 p; //position
    vec3 v; //velocity
    vec3 color; //color
    vec3 force; //force
    float r; //radius
    float m; //mass
};

vector<Ball> balls;

void addBall(vec3 leftBottom, vec3 rightTop)
{
    int random = 0;
    int randRange = 100;
    Ball newBall;

    vec3 _pos(0, 0, 0);
    vec3 _vel(0, 0, 0);
    vec3 _color(0, 0, 0);
    float m = 1;
    for (int i = 0; i < 3; i++)
    {
        float value = (float)(rand() % randRange) / (float)randRange;
        _pos[i] = leftBottom[i] + (rightTop[i] - leftBottom[i]) * value;
        _vel[i] = value * 2.f -1.f;
        _color[i] = value;
        m = 1 + value;
    }
    newBall.p = _pos;
    newBall.v = _vel;
    newBall.color = _color;
    newBall.force = vec3(0, 0, 0);
    newBall.m = m;
    newBall.r = m/2.f;

    balls.push_back(newBall);
}

void init() {
    srand(time(NULL));

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 800 / 600, -0.1f, 100.0f);

    //get window size
    windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    //Init camera
    vec3 center(0, 40, -5);
    vec3 at(0, 0, -5);
    vec3 up = vec3(0, 0, 1);
    myCamera.InitCamera(center, at, up);

    //Init variables
    addBall(vec3(-10, -10, -5), vec3(10, 10, 20));
    currentMouse = preMouse;
    mouseMoving = false;
}


void InitLight() {
    GLfloat light0_ambient[] = { 0.5, 0.5, 0.5, 1.0 };     //조명 특성
    GLfloat light0_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat light0_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat LightPosition[] = { 0.0, 0.0, 5.0, 1.0 };

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);    //구로 셰이딩
    glEnable(GL_DEPTH_TEST);    //깊이 버퍼 활성화
    glEnable(GL_LIGHTING);      //조명 활성화
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient); //주변광 설정
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse); //확산광 설정
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular); //반사광 설정
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
}


void MyMouse(int button, int state, int x, int y) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            preMouse = vec2(x, y);
            currentMouse = preMouse;
            mouseMoving = true;
        }
        else
            mouseMoving = false;
        break;
    case GLUT_MIDDLE_BUTTON:
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN) {
            addBall(vec3(-10, -10, -5), vec3(10, 10, 20));
        }
        break;
    default:
        break;
    }
    

    glutPostRedisplay();
}

void MyMouseMove(GLint X, GLint Y)
{
    currentMouse = vec2(X, Y);
    //Rotate camera
    if (mouseMoving)
    {
        float x_move = -100.f * (currentMouse[0] - preMouse[0]) / windowWidth;
        float y_move = -100.f * (currentMouse[1] - preMouse[1]) / windowHeight;

        myCamera.RotateCamera(myCamera.right, (float)y_move);
        myCamera.RotateCamera(myCamera.up, (float)x_move);
    }
    
    preMouse = currentMouse;

    glutPostRedisplay();
}



void MyKeyboard(unsigned char key, int x, int y) {
    float scale = 1;
    switch (key) {
    case 't':
        animate = !animate;
        break;
    case 'q':
        myCamera.MoveCamera(myCamera.up * scale);
        break;
    case 'z':
        myCamera.MoveCamera(myCamera.up * -scale);
        break;
    case 'w':
        myCamera.MoveCamera(myCamera.forward * scale);
        break;
    case 's':
        myCamera.MoveCamera(myCamera.forward * -scale);
        break;
    case 'a':
        myCamera.MoveCamera(myCamera.right * -scale);
        break;
    case 'd':
        myCamera.MoveCamera(myCamera.right * scale);
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

void DrawFloor(float size,  float level) {
    glBegin(GL_QUADS);
        glVertex3f(-size, -size, level);
        glVertex3f(size, -size, level);
        glVertex3f(size, size, level);
        glVertex3f(-size, size, level);
    glEnd();
}

void Render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //material
    GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat high_shininess[] = { 100.0 };


    //get camera variables from camera class
    vec3 eye = myCamera.eye;
    vec3 at = myCamera.at;
    vec3 up = myCamera.up;

    //set view transform matrix
    gluLookAt(eye[0], eye[1], eye[2], at[0], at[1], at[2], up[0], up[1], up[2]);

    for (int i = 0; i < balls.size(); i++)
    {
        glPushMatrix();
            glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
            glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
            
            glTranslatef(balls[i].p[0], balls[i].p[1], balls[i].p[2]);
            glColor3f(balls[i].color[0], balls[i].color[1], balls[i].color[2]);
            glutSolidSphere(balls[i].r, 50, 50);
        glPopMatrix();
    }

    glPushMatrix();
        glColor3f(0.0, 0.0, 0.8);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
        DrawFloor(30, -10);
    glPopMatrix();



    //Swap buffers
    glutSwapBuffers();
}

void MyReshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
}

void MyTimer(int Value) {
    //update balls
    float dt = 0.1;
    if (animate)
    {
        for (int i = 0; i < balls.size(); i++)
        {
            //add gradient force
           
            //update velocity
            
            //update position
            
            //modify position and velocity according to constraints

            //clear force
        }
    }
    

    glutPostRedisplay();
    glutTimerFunc(30, MyTimer, 1);
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Bouncing ball");
    init();
    InitLight();
    glutDisplayFunc(Render);
    glutReshapeFunc(MyReshape);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMouseMove);
    glutKeyboardFunc(MyKeyboard);
    glutTimerFunc(30, MyTimer, 1);

    glutMainLoop();
}