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
bool mouseSelect = false;
int selectedPoint = -1;
vec3 mouseIn3D;

camera myCamera;

struct Ball {
    vec3 p; //position
    vec3 v; //velocity
    vec3 color; //color
    vec3 force; //force
    float r; //radius
    float m; //mass
};

struct Spring {
    float length;
    ivec2 edge;
};

vector<ivec2> edges;

vector<Spring> springs;
vector<int> fixedConstraints;
vector<Ball> balls;

void MakeCloth(vec3 start, vec3 dX, vec3 dY, int xSeg, int ySeg)
{
    float mass = 1;
    float radius = 0.3;

    //generate positions
    for (int i = 0; i < ySeg; i++)
    {
        for (int j = 0; j < xSeg; j++)
        {
            vec3 point = start + dY * (float)i + dX * (float)j;
            Ball temp;
            temp.m = mass;
            temp.r = radius;
            temp.color = vec3(0, 0, 0);
            temp.v = vec3(0, 0, 0);
            temp.p = point;
            temp.force = vec3(0, 0, 0);
            balls.push_back(temp);
        }
    }

    edges.clear();
    //generate edges
    for (int i = 0; i < ySeg-1; i++)
    {
        for (int j = 0; j < xSeg-1; j++)
        {
            Spring temp;
            ivec2 edge;
            int idx = xSeg * i + j;

            //j 
            edge = ivec2(idx, idx + 1);
            temp.edge = edge;
            temp.length = length(balls[idx].p - balls[idx+1].p);
            springs.push_back(temp);
            //i 
            edge = ivec2(idx, idx + xSeg);
            temp.edge = edge;
            temp.length = length(balls[idx].p - balls[idx + xSeg].p);
            springs.push_back(temp);

            //i, j
            edge = ivec2(idx, idx + xSeg + 1);
            temp.edge = edge;
            temp.length = length(balls[idx].p - balls[idx + xSeg + 1].p);
            springs.push_back(temp);

            //i, j
            edge = ivec2(idx+1, idx + xSeg );
            temp.edge = edge;
            temp.length = length(balls[idx +1 ].p - balls[idx + xSeg].p);
            springs.push_back(temp);

            if (j == xSeg - 2)
            {
                edge = ivec2(idx +1 , idx + xSeg + 1);
                temp.edge = edge;
                temp.length = length(balls[idx + 1].p - balls[idx + xSeg +1].p);
                springs.push_back(temp);
            }

            if (i == ySeg - 2)
            {
                edge = ivec2(idx + xSeg, idx + xSeg + 1);
                temp.edge = edge;
                temp.length = length(balls[idx + xSeg].p - balls[idx + xSeg + 1].p);
                springs.push_back(temp);
            }
        }
    }

    //add constraints
     fixedConstraints.clear();
    for (int i = 0; i < xSeg; i++)
    {
        fixedConstraints.push_back(i);
    }
}

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
        m = 1 + value * 5;
    }
    newBall.p = _pos;
    newBall.v = _vel;
    newBall.color = _color;
    newBall.force = vec3(0, 0, 0);
    newBall.m = m;
    newBall.r = m/2.f;

    balls.push_back(newBall);
}

void addBall(vec3 pos)
{
    int random = 0;
    int randRange = 100;
    Ball newBall;

    vec3 _vel(0, 0, 0);
    vec3 _color(0, 0, 0);
    float m = 1;
    for (int i = 0; i < 3; i++)
    {
        float value = (float)(rand() % randRange) / (float)randRange;
        _vel[i] = value * 2.f - 1.f;
        _color[i] = value;
        m = 1 + value * 2;
    }
    newBall.p = pos;
    newBall.v = _vel;
    newBall.color = _color;
    newBall.force = vec3(0, 0, 0);
    newBall.m = m;
    newBall.r = m / 2.f;

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
    springs.clear();
    //addBall(vec3(-10, -10, -5), vec3(10, 10, 20));
    MakeCloth(vec3(10, 0, 10), vec3(-2, 0, 0), vec3(0, 1, -1), 10, 10);
   
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

int findClosestPoint(vec3 p)
{
    int index = -1;
    float closestDis = 100000000000;
    for (int i = 0; i < balls.size(); i++)
    {
        float dis = length(balls[i].p - p);
        if (dis < closestDis)
        {
            index = i;
            closestDis = dis;
        }
    }
    return index;
}

vec3 ConvertMouseTo3DPos(vec2 pos, float depth)
{
    //set vec4 by mouse position in normalized coordinate (clip)
    vec4 p((float)pos[0] / (float)windowWidth * 2.0 - 1, ((float)windowHeight - (float)pos[1]) / (float)windowHeight * 2.0 - 1, depth, 1.f);

    //convert it to world coordinate
    mat4 N = perspective(radians(45.f), (float)windowWidth / (float)windowHeight, 1.f, 200.f);
    mat4 V = lookAt(myCamera.eye, myCamera.at, myCamera.up);

    mat4 M = N * V;
    mat4 invM = inverse(M);
    vec4 convP = invM * p;
    convP /= convP[3];
    vec3 finalP = convP;

    return finalP;
}

void MyMouse(int button, int state, int x, int y) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            preMouse = vec2(x, y);
            currentMouse = preMouse;
            mouseMoving = true;
            if (mouseSelect)
            {
                mouseIn3D = ConvertMouseTo3DPos(vec2(x,y), 0.9);
                selectedPoint = findClosestPoint(mouseIn3D);
            }
        }
        else {
            mouseMoving = false;
            selectedPoint = -1;
        }
            
        break;
    case GLUT_MIDDLE_BUTTON:
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN) {
            if (!mouseSelect) {
                vec3 finalP = ConvertMouseTo3DPos(vec2(x, y), 0.9);
                addBall(finalP);
            }
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
    if (mouseMoving && !mouseSelect)
    {
        float x_move = -100.f * (currentMouse[0] - preMouse[0]) / windowWidth;
        float y_move = -100.f * (currentMouse[1] - preMouse[1]) / windowHeight;

        myCamera.RotateCamera(myCamera.right, (float)y_move);
        myCamera.RotateCamera(myCamera.up, (float)x_move);
    }
    
    if (mouseSelect)
    {
        mouseIn3D = ConvertMouseTo3DPos(vec2(X, Y), 0.9);
    }
    preMouse = currentMouse;

    glutPostRedisplay();
}



void MyKeyboard(unsigned char key, int x, int y) {
    float scale = 0.5;
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
    case 'x':
        mouseSelect = !mouseSelect;
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

void DrawSprings(float width)
{
    
    glBegin(GL_LINES);
    glLineWidth(width);
    for (int i = 0; i < springs.size(); i++)
    {
        vec3 p1, p2;
        p1 = balls[springs[i].edge[0]].p;
        p2 = balls[springs[i].edge[1]].p;
        glVertex3f(p1[0], p1[1], p1[2]);
        glVertex3f(p2[0], p2[1], p2[2]);
    }
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
    if (selectedPoint > -1)
    {
        glPushMatrix();
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

        glTranslatef(balls[selectedPoint].p[0], balls[selectedPoint].p[1], balls[selectedPoint].p[2]);
        glColor3f(1, 0, 0);
        glutSolidSphere(balls[selectedPoint].r *1.1f, 50, 50);
        glPopMatrix();
    }

    glPushMatrix();
        glColor3f(0.0, 0.0, 0.8);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
        DrawFloor(300, -10);
    glPopMatrix();
    glColor3f(0, 0, 0);
    DrawSprings(5);

    if (mouseSelect && selectedPoint > -1)
    {
        glPushMatrix();
        glColor3f(0, 0, 0);
        glTranslatef(mouseIn3D[0], mouseIn3D[1], mouseIn3D[2]);
        glutWireSphere(0.3, 20, 20);
        glPopMatrix();
    }
    //Swap buffers
    glutSwapBuffers();
}

void MyReshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 200.0);
}

void AddContactForce(float stiff) {
    for (int i = 0; i < balls.size(); i++)
    {
        for (int j = i+1; j < balls.size(); j++)
        {
            vec3 dis = balls[i].p - balls[j].p;
            //compute length
            float L = length(dis);

            //normalize the vector
            dis = normalize(dis);

            if (L < balls[i].r + balls[j].r)
            {
                //compute and add force
                vec3 force = stiff * ((balls[i].r + balls[j].r)-L)* dis;
                balls[i].force += force;
                balls[j].force -= force;
            }
        }
    }
}

void AddElasticForce(float stiff)
{
    for (int i = 0; i < springs.size(); i++)
    {
        vec3 p1 = balls[springs[i].edge[0]].p;
        vec3 p2 = balls[springs[i].edge[1]].p;

        vec3 dis = p2 - p1;
        float L = length(dis);
        dis = normalize(dis);

        vec3 elasticForce = stiff* (L - springs[i].length)* dis;
        balls[springs[i].edge[0]].force += elasticForce;
        balls[springs[i].edge[1]].force -= elasticForce;
    }

    for (int i = 0; i < balls.size(); i++)
    {
        balls[i].force -= balls[i].v * 0.01f;
    }
}

bool CheckFixed(int index)
{
    for (int i = 0; i < fixedConstraints.size(); i++)
    {
        if (index == fixedConstraints[i])
            return false;
    }
    return true;
}

void MyTimer(int Value) {
    //update balls
    float dt = 0.1;
    if (animate)
    {
        for (int i = 0; i < balls.size(); i++)
        {
            if (CheckFixed(i))
            {
                //add gravity force
                vec3 gravity(0, 0, -9.8);
                balls[i].force += gravity * balls[i].m;

                //add contact force
                AddContactForce(10);

                AddElasticForce(0.5);


                if (selectedPoint > -1 && mouseSelect)
                {
                    vec3 dis = mouseIn3D - balls[selectedPoint].p;
                    float L = length(dis);
                    dis = normalize(dis);
                    balls[selectedPoint].force += dis * L * balls[selectedPoint].m * 0.1f;
                }

                //update velocity
                balls[i].v += balls[i].force / balls[i].m * dt;

                //update position
                balls[i].p += balls[i].v * dt;

                //modify position according to constraints
                if (balls[i].p[2] - balls[i].r < -10) {
                    balls[i].p[2] = -10 + balls[i].r;
                    balls[i].v *= 0.9;
                    balls[i].v[2] *= -1;
                }

                //clear force
                balls[i].force = vec3(0, 0, 0);
            }    
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