#include <windows.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "glaux.h"
#include <math.h>

GLuint g_textureID = -1;


float g_fDistance = -4.5f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;


static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

void loadTexture(LPSTR  filename) {
    AUX_RGBImageRec* pTextureImage = auxDIBImageLoad(filename);

    if (pTextureImage != NULL) {
        glGenTextures(1, &g_textureID);

        glBindTexture(GL_TEXTURE_2D, g_textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, 3, pTextureImage->sizeX, pTextureImage->sizeY, 0,
            GL_RGB, GL_UNSIGNED_BYTE, pTextureImage->data);
    }

    if (pTextureImage) {
        if (pTextureImage->data)
            free(pTextureImage->data);

        free(pTextureImage);
    }
}

void MyMouse(int button, int state, int x, int y) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            ptLastMousePosit.x = ptCurrentMousePosit.x = x;
            ptLastMousePosit.y = ptCurrentMousePosit.y = y;
            bMousing = true;
        }
        else
            bMousing = false;
        break;
    case GLUT_MIDDLE_BUTTON:
    case GLUT_RIGHT_BUTTON:
        break;
    default:
        break;
    }

    glutPostRedisplay();
}

void MyMotion(int x, int y) {
    ptCurrentMousePosit.x = x;
    ptCurrentMousePosit.y = y;

    if (bMousing)
    {
        g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
        g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
    }

    ptLastMousePosit.x = ptCurrentMousePosit.x;
    ptLastMousePosit.y = ptCurrentMousePosit.y;

    glutPostRedisplay();
}

void init(void) {
    loadTexture("Data/homework.bmp");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
}



void texturedSphere(float radius, int slices) {
    GLUquadric* obj = gluNewQuadric();
    gluQuadricTexture(obj, GL_TRUE);
    gluQuadricNormals(obj, GLU_SMOOTH);
    gluSphere(obj, radius, slices, slices);
    gluDeleteQuadric(obj);
}

void texturedCube(float size) {
    //glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    //glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glutSolidCube(1);
}

void manualtexturedCube2()
{
    glBegin(GL_QUADS);

    glTexCoord2f(0, 0); glVertex3f(-1, -1, 1);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 1);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 1);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 1);

    glEnd();
}

void manualtexturedCube()
{


    // z=1
    glPushMatrix();

    glTranslatef(0, 0, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
    glTexCoord2f(1, 0); glVertex3f(1, -1, 0);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
    glTexCoord2f(0, 1); glVertex3f(-1, 1, 0);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    glRotatef(180, 1, 0, 0);
    glTranslatef(0, 0, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
    glTexCoord2f(1, 0); glVertex3f(1, -1, 0);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
    glTexCoord2f(0, 1); glVertex3f(-1, 1, 0);
    glEnd();
    glPopMatrix();

    // y=1
    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0, 0, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
    glTexCoord2f(1, 0); glVertex3f(1, -1, 0);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
    glTexCoord2f(0, 1); glVertex3f(-1, 1, 0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glRotatef(90, 1, 0, 0);
    glTranslatef(0, 0, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
    glTexCoord2f(1, 0); glVertex3f(1, -1, 0);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
    glTexCoord2f(0, 1); glVertex3f(-1, 1, 0);
    glEnd();
    glPopMatrix();

    // x=1
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
    glTexCoord2f(1, 0); glVertex3f(1, -1, 0);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
    glTexCoord2f(0, 1); glVertex3f(-1, 1, 0);
    glEnd();
    glPopMatrix();

    // x=-1
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    glTranslatef(0, 0, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(-1, -1, 0);
    glTexCoord2f(1, 0); glVertex3f(1, -1, 0);
    glTexCoord2f(1, 1); glVertex3f(1, 1, 0);
    glTexCoord2f(0, 1); glVertex3f(-1, 1, 0);
    glEnd();
    glPopMatrix();

}

void render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    

    glBindTexture(GL_TEXTURE_2D, g_textureID);
    glTranslatef(0.0f, 0.0f, g_fDistance);
    glRotatef(-g_fSpinY, 1.0f, 0.0f, 0.0f);
    glRotatef(-g_fSpinX, 0.0f, 1.0f, 0.0f);
    //Draw object
    //texturedSphere(1, 50);

    manualtexturedCube();
    glutSwapBuffers();
}



void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Texture Mapping - Passivity");
    init();
    glutDisplayFunc(render);
    glutReshapeFunc(MyReshape);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMotion);

    glutMainLoop();
}