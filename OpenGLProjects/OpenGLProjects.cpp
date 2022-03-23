// 검은 바탕에 흰색 사각형 그리기


#include <GL/glew.h>
#include <gl/freeglut.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <GL/glut.h>

#include <vector>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>


using namespace std;
using namespace glm;

#define PI 3.1415




void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_POLYGON);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glEnd();
    glFlush();
}

void DrawBox()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINES);
        //0-1
        glVertex3f(-0.5,-0.5,0);
        glVertex3f(-0.5, 0.5, 0);

        //1-2
        glVertex3f(-0.5, 0.5, 0);
        glVertex3f(0.5, 0.5, 0);

        //2-3
        glVertex3f(0.5, 0.5, 0);
        glVertex3f(0.5, -0.5, 0);

        //3-0
        glVertex3f(0.5, -0.5, 0);
        glVertex3f(-0.5, -0.5, 0);
    glEnd();
    glFlush();
}
void DrawBox2() {
    
    vector<fvec3> vertices;
    vertices.push_back(fvec3(-0.5, -0.5, 0));
    vertices.push_back(fvec3(-0.5, 0.5, 0));
    vertices.push_back(fvec3(0.5, 0.5, 0));
    vertices.push_back(fvec3(0.5, -0.5, 0));

    vector<ivec2> edges;
    edges.push_back(ivec2(0, 1));
    edges.push_back(ivec2(1, 2));
    edges.push_back(ivec2(2, 3));
    edges.push_back(ivec2(3, 0));

    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINES);
    for (int i = 0; i < edges.size(); i++)
    {
        vec3 a, b;
        a = vertices[edges[i][0]];
        b = vertices[edges[i][1]];

        glVertex3f(a[0], a[1], a[2]);
        glVertex3f(b[0], b[1], b[2]);
    }
    glEnd();
    glFlush();
}

void DrawCoordinate(float length)
{
    glBegin(GL_LINES);
    //x axis
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(length, 0, 0);

    //y axis
    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, length, 0);

    glEnd();
}




void DrawPoint(float radius, vec3 color, vec3 p)
{
    //set point parameters
    glColor3f(color[0], color[1], color[2]);
    glPointSize(radius);

    //draw point
    glBegin(GL_POINTS);
    glVertex2f(p[0], p[1]);
    glEnd();
}

void DrawBox(vec3 center, vec3 color, float size)
{
    //set point parameters
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_LINES);
        //0-1
        glVertex3f(center[0] - size / 2, center[1] - size / 2, 0);
        glVertex3f(center[0] - size / 2, center[1] + size / 2, 0);

        //1-2
        glVertex3f(center[0] - size / 2, center[1] + size / 2, 0);
        glVertex3f(center[0] + size / 2, center[1] + size / 2, 0);

        //2-3
        glVertex3f(center[0] + size / 2, center[1] + size / 2, 0);
        glVertex3f(center[0] + size / 2, center[1] - size / 2, 0);

        //3-0
        glVertex3f(center[0] + size / 2, center[1] - size / 2, 0);
        glVertex3f(center[0] - size / 2, center[1] - size / 2, 0);
    glEnd();
}


void DrawBox(vector<vec3> points, vec3 color)
{
    //set point parameters
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_LINES);

    for (int i = 0; i < 3; i++)
    {
        glVertex3f(points[i][0], points[i][1], 0);
        glVertex3f(points[i+1][0], points[i+1][1], 0);
    }
 
    glVertex3f(points[3][0], points[3][1], 0);
    glVertex3f(points[0][0], points[0][1], 0);

    glEnd();
}

void DrawBox(vector<vec4> points, vec3 color)
{
    //set point parameters
    glColor3f(color[0], color[1], color[2]);
    glBegin(GL_LINES);

    for (int i = 0; i < 3; i++)
    {
        glVertex3f(points[i][0], points[i][1], 0);
        glVertex3f(points[i + 1][0], points[i + 1][1], 0);
    }

    glVertex3f(points[3][0], points[3][1], 0);
    glVertex3f(points[0][0], points[0][1], 0);

    glEnd();
}


void DrawScene()
{
    //clear buffer
    glClear(GL_COLOR_BUFFER_BIT);

    //Transform point
    vec3 p(0, 0, 1);

    DrawPoint(10, vec3(1, 1, 1), p);

    //translation matrix
    mat3 T(1, 0, 0,
        0, 1, 0.5,
        0, 0, 1);

    //rotation matrix
    mat3 R(cos(45 * PI / 180), -sin(45 * PI / 180), 0,
        sin(45 * PI / 180), cos(45 * PI / 180), 0,
        0, 0, 1);

    //transform
    vec3 transformed_p;
    transformed_p = transpose(R) * transpose(T) * p;
    DrawPoint(10, vec3(1, 0, 0), transformed_p);

    transformed_p = transpose(T) * transpose(R) *  p;
    DrawPoint(10, vec3(0, 0, 1), transformed_p);

    //send it to buffer
    glFlush();
}

void DrawScene3()
{
    //clear buffer
    glClear(GL_COLOR_BUFFER_BIT);

    //Transform point
    vec4 p(0, 0, 0, 1);

    DrawPoint(10, vec3(1, 1, 1), p);

    //translation matrix
    mat4 trans = translate(mat4(1), vec3(0, 0.5, 0));

    //rotation matrix
    mat4 rot = rotate(mat4(1), radians(45.f), vec3(0, 0, 1));

    //transform
    vec3 transformed_p;
    transformed_p = trans * rot * p;
    DrawPoint(10, vec3(1, 0, 0), transformed_p);

    transformed_p = rot * trans * p;
    DrawPoint(10, vec3(0, 0, 1), transformed_p);

    //send it to buffer
    glFlush();
}

//Use gl Matrix
void DrawScene4()
{
    //clear buffer
    glClear(GL_COLOR_BUFFER_BIT);

    //point
    vec4 p(0, 0, 0, 1);
    DrawPoint(10, vec3(1, 1, 1), p);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Move coordinate by (0, 0.5, 0)
    glTranslatef(0, 0.5, 0);
    DrawPoint(10, vec3(1, 0, 0), p);

    //Rotate coordiante by 45
    glRotatef(45.f,0,0,1);
    DrawPoint(10, vec3(0, 0, 1), p);

    //Move coordinate by (-0.5, 0, 0)
    glTranslatef(-0.5, 0, 0);
    DrawPoint(10, vec3(0, 1, 0), p);

    //send it to buffer
    glFlush();
}

//Use gl and box test
void DrawScene5()
{
    //clear buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //point
    vec4 p(0, 0, 0, 1);
    DrawBox(p, vec3(1, 1, 1), 0.5);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Move coordinate by (0, 0.5, 0)
    glTranslatef(0, 0.5, 0);
    DrawBox(p, vec3(1, 0, 0), 0.5);

    //Rotate coordiante by 45
    glRotatef(45.f, 0, 0, 1);
    DrawBox(p, vec3(0, 0, 1), 0.5);

    //Move coordinate by (-0.5, 0, 0)
    glTranslatef(-0.5, 0, 0);
    DrawBox(p, vec3(0, 1, 0), 0.5);

    //send it to buffer
    glFlush();
}

//좌표을 잘 보여주기 위한 예제
void DrawScene6()
{
    //clear buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //point
    vec4 p(0, 0, 0, 1);
    //DrawBox(p, vec3(1, 1, 1), 0.5);
    DrawPoint(10, vec3(1, 1, 1), vec3(0.1, 0, 0));

    DrawCoordinate(0.2);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Move coordinate by (0, 0.5, 0)
    glTranslatef(0, 0.5, 0);
    //DrawBox(p, vec3(1, 0, 0), 0.5);
    DrawPoint(10, vec3(1, 0, 0), vec3(0.1, 0, 0));

    DrawCoordinate(0.2);

    //Rotate coordiante by 45
    glRotatef(45.f, 0, 0, 1);
    //DrawBox(p, vec3(0, 0, 1), 0.5);
    DrawPoint(10, vec3(0, 0, 1), vec3(0.1, 0, 0));

    DrawCoordinate(0.2);

    //Move coordinate by (-0.5, 0, 0)
    glTranslatef(-0.5, 0, 0);
    //DrawBox(p, vec3(0, 1, 0), 0.5);
    DrawPoint(10, vec3(0, 1, 0), vec3(0.1, 0, 0));

    DrawCoordinate(0.2);

    //send it to buffer
    glFlush();
}

//좌표 변환과 점 변환의 관계
void HW2()
{
    //clear buffer
    glClear(GL_COLOR_BUFFER_BIT);

    //define point
    float size = 0.2;
    vector<vec4> points;
    points.push_back(vec4(-size/2, -size/2, 0, 1));
    points.push_back(vec4(-size / 2, size / 2, 0, 1));
    points.push_back(vec4(size / 2, size / 2, 0, 1));
    points.push_back(vec4(size / 2, -size / 2, 0, 1));
    DrawBox(points, vec3(1, 1, 1));

    int mode = 2;
    if (mode == 0) {
        //translation matrix
        mat4 xtrans = translate(mat4(1), vec3(0.3, 0, 0));
        mat4 ytrans = translate(mat4(1), vec3(0, 0.4, 0));

        //rotation matrix
        mat4 rot = rotate(mat4(1), radians(30.f), vec3(0, 0, 1));

        //Move point by (-0.3, 0, 0)
        for (int i=0;i<points.size();i++){
            points[i] = xtrans * points[i];
        }
        DrawBox(points, vec3(1, 0, 0));

        //Rotate point by 30
        for (int i = 0; i < points.size(); i++) {
            points[i] = rot * points[i];
        }
        DrawBox(points, vec3(0, 0, 1));

        //Move point by (0, 0.4, 0)
        for (int i = 0; i < points.size(); i++) {
            points[i] = ytrans * points[i];
        }
        DrawBox(points, vec3(0, 1, 0));
    }
    else if(mode == 1) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //Move coordinate by (0, 0.5, 0)
        glTranslatef(0, 0.4, 0);
        DrawCoordinate(0.2);
        DrawBox(points, vec3(1, 0, 0));

        //Rotate coordiante by 45
        glRotatef(30.f, 0, 0, 1);
        DrawCoordinate(0.2);
        DrawBox(points, vec3(0, 0, 1));

        //Move coordinate by (-0.5, 0, 0)
        glTranslatef(0.3, 0, 0);
        DrawCoordinate(0.2);
        DrawBox(points, vec3(0, 1, 0));
    }
    else {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //Move coordinate by (0, 0.5, 0)
        glTranslatef(0.3, 0, 0);
        
        DrawCoordinate(0.2);
        DrawBox(points, vec3(1, 0, 0));

        //Rotate coordiante by 45
        glRotatef(30.f, 0, 0, 1);
        DrawCoordinate(0.2);
        DrawBox(points, vec3(0, 0, 1));

        //Move coordinate by (-0.5, 0, 0)
        glTranslatef(0, 0.4, 0);
        DrawCoordinate(0.2);
        DrawBox(points, vec3(0, 1, 0));
    }

    //send it to buffer
    glFlush();
}

void MyReshape(int NewWidth, int NewHeight) {
    glViewport(0, 0, NewWidth, NewHeight);
    GLfloat WidthFactor = (GLfloat)NewWidth / (GLfloat)300;
    GLfloat HeightFactor = (GLfloat)NewHeight / (GLfloat)300;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0 * WidthFactor, 1.0 * WidthFactor,
        -1.0 * HeightFactor, 1.0 * HeightFactor, -1.0, 1.0);
}


//좌표 변환과 점 변환의 관계
void DrawScene7()
{
    //clear buffer
    glClear(GL_COLOR_BUFFER_BIT);

    //define point
    vec4 p(0, 0, 0, 1);
    DrawPoint(10, vec3(1, 1, 1), p);

    bool modelTransform = true;
    if (modelTransform) {
        //translation matrix
        mat4 xtrans = translate(mat4(1), vec3(-0.5, 0, 0));
        mat4 ytrans = translate(mat4(1), vec3(0, 0.5, 0));

        //rotation matrix
        mat4 rot = rotate(mat4(1), radians(45.f), vec3(0, 0, 1));

        //Move point by (-0.5, 0, 0)
        p = xtrans * p;
        DrawCoordinate(0.2);
        DrawPoint(10, vec3(1, 0, 0), p);

        //Rotate point by 45
        p = rot * p;
        DrawCoordinate(0.2);
        DrawPoint(10, vec3(0, 0, 1), p);

        //Move point by (0, 0.5, 0)
        p = ytrans * p;
        DrawCoordinate(0.2);
        DrawPoint(10, vec3(0, 1, 0), p);
    }
    else {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //Move coordinate by (0, 0.5, 0)
        glTranslatef(0, 0.5, 0);
        DrawCoordinate(0.2);
        DrawPoint(10, vec3(1, 0, 0), p);

        //Rotate coordiante by 45
        glRotatef(45.f, 0, 0, 1);
        DrawCoordinate(0.2);
        DrawPoint(10, vec3(0, 0, 1), p);

        //Move coordinate by (-0.5, 0, 0)
        glTranslatef(-0.5, 0, 0);
        DrawCoordinate(0.2);
        DrawPoint(10, vec3(0, 1, 0), p);
    }

    //send it to buffer
    glFlush();
}


//뷰 행렬 변환
void DrawScene8() {
    //clear buffer
    glClear(GL_COLOR_BUFFER_BIT);

    vec3 eye(0.4, 0, 0.1);
    vec3 at(0.4, 0, -100);
    vec3 up(0,1,0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye[0],eye[1],eye[2],
        at[0],at[1],at[2],
        up[0], up[1], up[2]);
    glRotatef(45, 0.0, 1.0, 0.0);
    glutWireSphere(0.2, 20, 16);
    
    //send it to buffer
    glFlush();
}
void GlmMatrixTest()
{
    vec4 myVec(1, 2, 3, 1);
   
    mat4 myMat = translate(mat4(1), vec3(1.f, 2.f, 3.f));

    vec4 transformed = myMat * myVec;
}

void DrawScene2()
{
    //clear buffer
    glClear(GL_COLOR_BUFFER_BIT);

    vector<vec3> points;
    points.clear();
    points.push_back(vec3(-0.5, -0.5, 1));
    points.push_back(vec3(-0.5, 0.5, 1));
    points.push_back(vec3(0.5, 0.5, 1));
    points.push_back(vec3(0.5, -0.5, 1));

    //translation matrix
    mat3x3 T(1, 0, 0.2,
        0, 1, 0.2,
        0, 0, 1);

    //rotation matrix
    mat3x3 R(cos(45 * PI / 180), sin(45 * PI / 180), 0,
        -sin(45 * PI / 180), cos(45 * PI / 180), 0,
        0, 0, 1);

    //transform
    //p = R*T*p
    //p' = p' * T' * R'
    DrawBox(points, vec3(1, 1, 1));

    for (int i = 0; i < points.size(); i++)
    {
        points[i] =  transpose(T) * transpose(R) * points[i];
    }
    DrawBox(points, vec3(1, 0, 0));

    //send it to buffer
    glFlush();
}

void DrawSquare()
{
    glBegin(GL_POLYGON);
    glVertex3f(-0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.0f);
    glVertex3f(0.5f, 0.5f, 0.0f);
    glVertex3f(-0.5f, 0.5f, 0.0f);
    glEnd();
}

void ViewportTest()
{
    //set background color
    glClearColor(1.0, 1.0, 1.0, 1.0);
   
    //clear buffer
    glClear(GL_COLOR_BUFFER_BIT);
    
    //set viewport
    glViewport(0, 0, 300, 300);

    //draw red square
    glColor3f(1, 0, 0);
    DrawSquare();

    //send it to buffer
    glFlush();
}



int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(500, 300);
    glutInitWindowPosition(0, 0); 
    glutCreateWindow("OpenGL Example Drawing");
    
    glutDisplayFunc(ViewportTest);
    //glutReshapeFunc(MyReshape);
    glutMainLoop();
    return 0;
}


