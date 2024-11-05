#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <iostream>


#include <vector>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>


float g_fDistance = -4.5f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;


static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

std::vector < glm::vec3 > vertices;
std::vector < glm::ivec3 > faces;
std::vector < glm::vec2 > uvs;
std::vector < glm::vec3 > normals;


// 조명
GLfloat light_amb[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_specular[] = { 1, 1, 1, 1.0 };



bool LoadObj(const char* path,
    std::vector < glm::vec3 >& out_vertices,
    std::vector < glm::ivec3 >& out_faces,
    std::vector < glm::vec2 >& out_uvs,
    std::vector < glm::vec3 >& out_normals)
{
    //init variables
    out_vertices.clear();
    out_faces.clear();
    out_uvs.clear();
    out_normals.clear();

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }

    while (1) {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        if (res == -1)
            break;

        if (strcmp(lineHeader, "v") == 0) {

        }
        else if (strcmp(lineHeader, "vt") == 0) {

        }
        else if (strcmp(lineHeader, "vn") == 0) {

        }
        else if (strcmp(lineHeader, "f") == 0) {

        }
    }

}



bool LoadPly(const char* path,
    std::vector < glm::vec3 >& out_vertices,
    std::vector < glm::ivec3 >& out_faces,
    std::vector < glm::vec3 >& out_normals)
{
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }

    //init variables
    out_vertices.clear();
    out_faces.clear();
    out_normals.clear();

    int nbVertices = 0;
    int nbFaces = 0;

    while (1) {
        char lineHeader1[128];
        char lineHeader2[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader1);
        if (res == -1)
            break;
        if (strcmp(lineHeader1, "element") == 0) {
            fscanf(file, "%s", lineHeader2);
            if (strcmp(lineHeader2, "vertex") == 0) {
                fscanf(file, "%d\n", &nbVertices);
            }
            else if (strcmp(lineHeader2, "face") == 0) {
                fscanf(file, "%d\n", &nbFaces);
            }
        }
        else if (strcmp(lineHeader1, "end_header") == 0) {
            break;
        }
    }

    for (int i = 0; i < nbVertices; i++)
    {

    }

    for (int i = 0; i < nbFaces; i++)
    {

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

void InitLight()
{
    GLfloat LightPosition[] = { 0.0, 2.0, 0.0, 1.0 };
    glEnable(GL_LIGHTING);      //조명 활성화
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb); //주변광 설정
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); //확산광 설정
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); //반사광 설정
    glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

}

void init(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glShadeModel(GL_SMOOTH);    //구로 셰이딩
    glEnable(GL_DEPTH_TEST); // 깊이버퍼
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_FRONT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
    InitLight();
}



void DrawSurface(std::vector < glm::vec3 >& vectices, 
    std::vector < glm::vec3 >& normals, 
    std::vector < glm::ivec3 >& faces)
{
    glBegin(GL_TRIANGLES);

    glEnd();
}

void DrawWireSurface(std::vector < glm::vec3 >& vectices,
    std::vector < glm::ivec3 >& faces)
{
    glBegin(GL_LINES);


    glEnd();
}



void render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glTranslatef(0.0f, 0.0f, g_fDistance);
    glRotatef(-g_fSpinY, 1.0f, 0.0f, 0.0f);
    glRotatef(-g_fSpinX, 0.0f, 1.0f, 0.0f);

    //Draw here

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

    //LoadObj("Data/bunny/bunny.obj", vertices, faces, uvs, normals);
    //LoadPly("Data/bunny/bun_zipper_res4.ply", vertices, faces, normals);

    glutDisplayFunc(render);
    glutReshapeFunc(MyReshape);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMotion);

    glutMainLoop();
}