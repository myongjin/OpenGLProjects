#include <windows.h>
#include <mmsystem.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "glaux.h"
#include "matrix4x4f.h"
#include "vector3f.h"
#include "bitmap_fonts.h"

int    g_nWindowWidth = 640;
int    g_nWindowHeight = 480;
GLuint g_mipMapTextureID = -1;

POINT  g_ptLastMousePosit;
POINT  g_ptCurrentMousePosit;
bool    g_bMousing = false;
float    g_fMoveSpeed = 25.0f;
float    g_fElpasedTime = 0.03f;
double  g_dCurTime;
double  g_dLastTime;

vector3f g_vEye(0.0f, 10.0f, 0.0f);
vector3f g_vLook(0.5f, -0.4f, -0.5f);
vector3f g_vUp(0.0f, 1.0f, 0.0f);
vector3f g_vRight(1.0f, 0.0f, 0.0f);

struct Vertex {
    float tu, tv;
    float x, y, z;
};

Vertex g_quadVertices[] = {
    { 0.0f,0.0f, -1.0f, 0.0f, -1.0f },
    { 1.0f,0.0f,  1.0f, 0.0f, -1.0f },
    { 1.0f,1.0f,  1.0f, 0.0f,  1.0f },
    { 0.0f,1.0f, -1.0f, 0.0f,  1.0f }
};

enum FilterTypes {
    FILTER_TYPE_NEAREST = 0,
    FILTER_TYPE_LINEAR,
    FILTER_TYPE_NEAREST_MIPMAP_NEAREST,
    FILTER_TYPE_LINEAR_MIPMAP_NEAREST,
    FILTER_TYPE_NEAREST_MIPMAP_LINEAR,
    FILTER_TYPE_LINEAR_MIPMAP_LINEAR
};

int	g_MinFilterType = FILTER_TYPE_LINEAR_MIPMAP_LINEAR;
int	g_MagFilterType = FILTER_TYPE_LINEAR;
bool g_bChangeFilters = true;

void loadMipMapTexture(void) {
    AUX_RGBImageRec* pTextureImage256base = auxDIBImageLoad("Data/texbase256.bmp");

    if (pTextureImage256base != NULL) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &g_mipMapTextureID);
        glBindTexture(GL_TEXTURE_2D, g_mipMapTextureID);
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pTextureImage256base->sizeX,
            pTextureImage256base->sizeY, GL_RGB, GL_UNSIGNED_BYTE,
            pTextureImage256base->data);
    }

    if (pTextureImage256base) {
        if (pTextureImage256base->data)
            free(pTextureImage256base->data);

        free(pTextureImage256base);
    }

    AUX_RGBImageRec* pTexArray[6];

    pTexArray[0] = auxDIBImageLoad("Data/tex256.bmp");
    pTexArray[1] = auxDIBImageLoad("Data/tex128.bmp");
    pTexArray[2] = auxDIBImageLoad("Data/tex64.bmp");
    pTexArray[3] = auxDIBImageLoad("Data/tex32.bmp");
    pTexArray[4] = auxDIBImageLoad("Data/tex16.bmp");
    pTexArray[5] = auxDIBImageLoad("Data/tex8.bmp");

    glTexImage2D(GL_TEXTURE_2D, 0, 3, pTexArray[0]->sizeX, pTexArray[0]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, pTexArray[0]->data);

    glTexImage2D(GL_TEXTURE_2D, 1, 3, pTexArray[1]->sizeX, pTexArray[1]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, pTexArray[1]->data);

    glTexImage2D(GL_TEXTURE_2D, 2, 3, pTexArray[2]->sizeX, pTexArray[2]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, pTexArray[2]->data);

    glTexImage2D(GL_TEXTURE_2D, 3, 3, pTexArray[3]->sizeX, pTexArray[3]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, pTexArray[3]->data);

    glTexImage2D(GL_TEXTURE_2D, 4, 3, pTexArray[4]->sizeX, pTexArray[4]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, pTexArray[4]->data);

    glTexImage2D(GL_TEXTURE_2D, 5, 3, pTexArray[5]->sizeX, pTexArray[5]->sizeY, 0,
        GL_RGB, GL_UNSIGNED_BYTE, pTexArray[5]->data);

    for (int i = 0; i < 6; ++i) {
        if (pTexArray[i]) {
            if (pTexArray[i]->data)
                free(pTexArray[i]->data);

            free(pTexArray[i]);
        }
    }
}

void MyInit(void) {
    loadMipMapTexture();

    glClearColor(0.35f, 0.53f, 0.7f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLdouble)g_nWindowWidth / g_nWindowHeight, 0.1, 100.0);
}

void updateViewMatrix(void) {
    matrix4x4f view;
    view.identity();

    g_vLook.normalize();
    g_vRight = crossProduct(g_vLook, g_vUp);
    g_vRight.normalize();
    g_vUp = crossProduct(g_vRight, g_vLook);
    g_vUp.normalize();

    view.m[0] = g_vRight.x;
    view.m[1] = g_vUp.x;
    view.m[2] = -g_vLook.x;
    view.m[3] = 0.0f;

    view.m[4] = g_vRight.y;
    view.m[5] = g_vUp.y;
    view.m[6] = -g_vLook.y;
    view.m[7] = 0.0f;

    view.m[8] = g_vRight.z;
    view.m[9] = g_vUp.z;
    view.m[10] = -g_vLook.z;
    view.m[11] = 0.0f;

    view.m[12] = -dotProduct(g_vRight, g_vEye);
    view.m[13] = -dotProduct(g_vUp, g_vEye);
    view.m[14] = dotProduct(g_vLook, g_vEye);
    view.m[15] = 1.0f;

    glMultMatrixf(view.m);
}

void setMinificationFilter(void) {
    if (g_MinFilterType == 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if (g_MinFilterType == 1)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    if (g_MinFilterType == 2)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_NEAREST_MIPMAP_NEAREST);

    if (g_MinFilterType == 3)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_NEAREST);

    if (g_MinFilterType == 4)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_NEAREST_MIPMAP_LINEAR);

    if (g_MinFilterType == 5)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
            GL_LINEAR_MIPMAP_LINEAR);
}

void setMagnificationFilter(void) {
    if (g_MagFilterType == 0)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (g_MagFilterType == 1)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void MyRender(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    updateViewMatrix();

    if (g_bChangeFilters == true) {
        setMinificationFilter();
        setMagnificationFilter();
        g_bChangeFilters = false;
    }

    glBindTexture(GL_TEXTURE_2D, g_mipMapTextureID);

    float x = 0.0f;
    float z = 0.0f;

    for (int i = 0; i < 25; ++i) {
        for (int j = 0; j < 25; ++j) {
            glPushMatrix();
            glTranslatef(x, 0.0f, z);
            glInterleavedArrays(GL_T2F_V3F, 0, g_quadVertices);
            glDrawArrays(GL_QUADS, 0, 4);
            glPopMatrix();

            x += 2.0f;
        }
        x = 0.0f;
        z -= 2.0f;
    }

    static char strMinFilter[255];
    static char strMagFilter[255];

    if (g_MinFilterType == FILTER_TYPE_NEAREST)
        sprintf_s(strMinFilter, "GL_TEXTURE_MIN_FILTER = GL_NEAREST    (Change: F1)");
    else if (g_MinFilterType == FILTER_TYPE_LINEAR)
        sprintf_s(strMinFilter, "GL_TEXTURE_MIN_FILTER = GL_LINEAR    (Change: F1)");
    else if (g_MinFilterType == FILTER_TYPE_NEAREST_MIPMAP_NEAREST)
        sprintf_s(strMinFilter, "GL_TEXTURE_MIN_FILTER = GL_NEAREST_MIPMAP_NEAREST (Change: F1)");
    else if (g_MinFilterType == FILTER_TYPE_LINEAR_MIPMAP_NEAREST)
        sprintf_s(strMinFilter, "GL_TEXTURE_MIN_FILTER = GL_LINEAR_MIPMAP_NEAREST (Change: F1)");
    else if (g_MinFilterType == FILTER_TYPE_NEAREST_MIPMAP_LINEAR)
        sprintf_s(strMinFilter, "GL_TEXTURE_MIN_FILTER = GL_NEAREST_MIPMAP_LINEAR (Change: F1)");
    else if (g_MinFilterType == FILTER_TYPE_LINEAR_MIPMAP_LINEAR)
        sprintf_s(strMinFilter, "GL_TEXTURE_MIN_FILTER = GL_LINEAR_MIPMAP_LINEAR (Change: F1)");

    if (g_MagFilterType == FILTER_TYPE_NEAREST)
        sprintf_s(strMagFilter, "GL_TEXTURE_MAG_FILTER = GL_NEAREST    (Change: F2)");
    else if (g_MagFilterType == FILTER_TYPE_LINEAR)
        sprintf_s(strMagFilter, "GL_TEXTURE_MAG_FILTER = GL_LINEAR    (Change: F2)");

    beginRenderText(g_nWindowWidth, g_nWindowHeight);
    {
        glColor3f(0.0f, 0.0f, 0.0f);
        renderText(5, 15, BITMAP_FONT_TYPE_HELVETICA_12, strMinFilter);
        renderText(5, 30, BITMAP_FONT_TYPE_HELVETICA_12, strMagFilter);
    }
    endRenderText();

    glutSwapBuffers();
}

void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
}

void MySpecial(int key, int x, int y) {
    vector3f tmpLook = g_vLook;
    vector3f tmpRight = g_vRight;

    if (key == GLUT_KEY_F1) {
        ++g_MinFilterType;
        if (g_MinFilterType > 5) g_MinFilterType = 0;
        g_bChangeFilters = true;
    }
    else if (key == GLUT_KEY_F2) {
        ++g_MagFilterType;
        if (g_MagFilterType > 1) g_MagFilterType = 0;
        g_bChangeFilters = true;
    }
    else if (key == GLUT_KEY_UP) {
        g_vEye -= tmpLook * -g_fMoveSpeed * g_fElpasedTime;
    }
    else if (key == GLUT_KEY_DOWN) {
        g_vEye += (tmpLook * -g_fMoveSpeed) * g_fElpasedTime;
    }
    else if (key == GLUT_KEY_LEFT) {
        g_vEye -= (tmpRight * g_fMoveSpeed) * g_fElpasedTime;
    }
    else if (key == GLUT_KEY_RIGHT) {
        g_vEye += (tmpRight * g_fMoveSpeed) * g_fElpasedTime;
    }
    else if (key == GLUT_KEY_HOME) {
        g_vEye.y += g_fMoveSpeed * g_fElpasedTime;
    }
    else if (key == GLUT_KEY_END) {
        g_vEye.y -= g_fMoveSpeed * g_fElpasedTime;
    }

    glutPostRedisplay();
}

void MyMouse(int button, int state, int x, int y) {
    switch (button) {
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
            g_ptLastMousePosit.x = g_ptCurrentMousePosit.x = x;
            g_ptLastMousePosit.y = g_ptCurrentMousePosit.y = y;
            g_bMousing = true;
        }
        else
            g_bMousing = false;
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
    g_ptCurrentMousePosit.x = x;
    g_ptCurrentMousePosit.y = y;

    matrix4x4f matRotation;

    if (g_bMousing) {
        int nXDiff = (g_ptCurrentMousePosit.x - g_ptLastMousePosit.x);
        int nYDiff = (g_ptCurrentMousePosit.y - g_ptLastMousePosit.y);

        if (nYDiff != 0) {
            matRotation.rotate(-(float)nYDiff / 3.0f, g_vRight);
            matRotation.transformVector(&g_vLook);
            matRotation.transformVector(&g_vUp);
        }

        if (nXDiff != 0) {
            matRotation.rotate(-(float)nXDiff / 3.0f, vector3f(0.0f, 1.0f, 0.0f));
            matRotation.transformVector(&g_vLook);
            matRotation.transformVector(&g_vUp);
        }
    }

    g_ptLastMousePosit.x = g_ptCurrentMousePosit.x;
    g_ptLastMousePosit.y = g_ptCurrentMousePosit.y;

    glutPostRedisplay();
}

void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Texture MipMapping");
    MyInit();
    glutDisplayFunc(MyRender);
    glutReshapeFunc(MyReshape);
    glutSpecialFunc(MySpecial);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMotion);

    glutMainLoop();
}