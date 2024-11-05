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

vec2 preMouse, currentMouse;
int windowHeight, windowWidth;

// 태양 조명
GLfloat sun_light_amb[] = { 0.5, 0, 0, 1.0 };    
GLfloat sun_light_diffuse[] = { 1, 0.5, 0.5, 1.0 };
GLfloat sun_light_specular[] = { 1, 1, 1, 1.0 };

// 달 조명
GLfloat moon_light_amb[] = { 0.5, 0.5, 0, 1.0 };    
GLfloat moon_light_diffuse[] = { 1, 1, 0.5, 1.0 };
GLfloat moon_light_specular[] = { 1, 1, 1, 1.0 };

//재질 설정
//태양
GLfloat sun_mat_amb[] = { 0.2, 0 , 0, 1.0 };
GLfloat sun_mat_diffuse[] = { 1, 0.5, 0.5, 1.0 };
GLfloat sun_mat_specular[] = { 0, 0, 0, 1 };
GLfloat sun_mat_emission[] = { 0.3, 0.1, 0.1, 0.0 };

//달
GLfloat moon_mat_amb[] = { 0.1, 0.1, 0.1, 1.0 };
GLfloat moon_mat_diff[] = { 0.5, 0.5, 0.1, 1.0 };
GLfloat moon_mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat moon_shininess[] = { 100.0 };
GLfloat moon_mat_emission[] = { 0.3, 0.3, 0.1, 0.0 };

//지구
GLfloat earth_mat_amb[] = { 0.1, 0.1, 0.1, 1.0 };
GLfloat earth_mat_diff[] = { 0.1, 0.1, 0.8, 1.0 };
GLfloat earth_mat_specular[] = { 0.5, 0.5, 1.0, 1.0 };
GLfloat earth_shininess[] = { 80.0 };


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
    //glColor3f(1.0, 0.3, 0.3);
    GLfloat LightPosition[] = { 0.0, 0.0, 0.0, 1.0 };

    //Draw sun
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
    glMaterialfv(GL_FRONT, GL_AMBIENT, sun_mat_amb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sun_mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sun_mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, sun_mat_emission);
    glColor3f(1.0, 0.3, 0.3);
    glutSolidSphere(0.2, 100, 100);
    
    
    glPushMatrix();
        glRotatef((GLfloat)Day, 0.0, 1.0, 0.0);
        glTranslatef(0.7, 0.0, 0.0);
        glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);

        //Draw earth
       
        glMaterialfv(GL_FRONT, GL_AMBIENT, earth_mat_amb);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, earth_mat_diff);
        glMaterialfv(GL_FRONT, GL_SPECULAR, earth_mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, earth_shininess);

        glColor3f(0.5, 0.6, 0.7);
        glutSolidSphere(0.1, 50, 50);

        glPushMatrix();
            glRotatef((GLfloat)Time, 0.0, 1.0, 0.0);
            glTranslatef(0.2, 0.0, 0.0);

            //Draw moon
            glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
            glMaterialfv(GL_FRONT, GL_AMBIENT, moon_mat_amb);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, moon_mat_diff);
            glMaterialfv(GL_FRONT, GL_EMISSION, moon_mat_emission);
            glMaterialfv(GL_FRONT, GL_SPECULAR, moon_mat_specular);
            glMaterialfv(GL_FRONT, GL_SHININESS, moon_shininess);

            
            glColor3f(0.9, 0.8, 0.2);
            glutSolidSphere(0.04, 20, 20);
        glPopMatrix();
    glPopMatrix();

}

void MyDisplay() {
//
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    //Rotate camera
    float x_move = -30.f * (currentMouse[0] - preMouse[0]) / windowWidth;
    float y_move = -30.f * (currentMouse[1] - preMouse[1]) / windowHeight;

    myCamera.RotateCamera(myCamera.right, (float)y_move);
    myCamera.RotateCamera(myCamera.up, (float)x_move);
    preMouse = currentMouse;

    //Set projection;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(20, (float)windowWidth / (float)windowHeight, 1.f, 10.f);

    
    //Draw Scene
    DrawSolarSystem();

    
    //Send it to buffer
    glFlush();
    //glDisable(GL_DEPTH_TEST);

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
        //myCamera.RotateCamera(myCamera.right, 1);
        myCamera.MoveCamera(myCamera.forward * scale);
        glutPostRedisplay();
        break;
    case 's':
        //myCamera.RotateCamera(myCamera.right, -1);
        myCamera.MoveCamera(myCamera.forward * -scale);
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


void InitLight()
{


    glShadeModel(GL_SMOOTH);    //구로 셰이딩
    glEnable(GL_DEPTH_TEST);    //깊이 버퍼 활성화
    glEnable(GL_LIGHTING);      //조명 활성화
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_amb); //주변광 설정
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse); //확산광 설정
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular); //반사광 설정


    glLightfv(GL_LIGHT1, GL_AMBIENT, moon_light_amb); //주변광 설정
    glLightfv(GL_LIGHT1, GL_DIFFUSE, moon_light_diffuse); //확산광 설정
    glLightfv(GL_LIGHT1, GL_SPECULAR, moon_light_specular); //반사광 설정
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


    InitLight();
    //set callback functions
    glutDisplayFunc(MyDisplay);
    glutKeyboardFunc(MyKeyboard);
    glutTimerFunc(40, MyTimer, 1);
    glutMouseFunc(MyMouseClick);
    glutMotionFunc(MyMouseMove);
    glutReshapeFunc(MyReshape);

    //start mainloop
    glutMainLoop();
    return 0;
}
