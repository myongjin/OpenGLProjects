#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut_ext.h>
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
GLfloat light_diffuse[] = { 1, 1, 1, 1.0 };
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
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
            out_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0) {
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y);
            out_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0) {
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            out_normals.push_back(normal);
        }
        else if (strcmp(lineHeader, "f") == 0) {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d//%d %d//%d %d//%d\n",
                &vertexIndex[0], &normalIndex[0],
                &vertexIndex[1], &normalIndex[1],
                &vertexIndex[2], &normalIndex[2]);

            out_faces.push_back(glm::ivec3(vertexIndex[0]-1, vertexIndex[1]-1, vertexIndex[2]-1));
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
        float values[5];
        fscanf(file, "%f %f %f %f %f\n", &values[0], &values[1], &values[2], &values[3], &values[4]);
        out_vertices.push_back(glm::vec3(values[0], values[1], values[2]));
    }

    for (int i = 0; i < nbFaces; i++)
    {
        int values[5];
        fscanf(file, "%d %d %d %d\n", &values[0], &values[1], &values[2], &values[3]);
        out_faces.push_back(glm::ivec3(values[1], values[2], values[3]));
    }

    std::vector<glm::vec3> triNormals;
    triNormals.clear();

    //삼각형 별로 normal 계산
    //face 수만큼 즉 삼각형 수만큼 반복
    for (int i = 0; i < out_faces.size(); i++)
    {
        //compute a tri normal
        glm::vec3 a = out_vertices[out_faces[i][0]];
        glm::vec3 b = out_vertices[out_faces[i][1]];
        glm::vec3 c = out_vertices[out_faces[i][2]];
        glm::vec3 ab = b - a;
        glm::vec3 ac = c - a;
        glm::vec3 tnormal = glm::cross(ab, ac);
        tnormal = glm::normalize(tnormal);

        //계산된 삼각형 normal을 배열 변수에 저장
        triNormals.push_back(tnormal);
    }

    //각 점이 속한 삼격형 Index 정보 저장을 위한 2차원 배열 변수
    std::vector < std::vector<int>> triList;
    triList.clear();

    //각 점이 속한 삼각형 찾기
    //나중에 각 점에서 평균 normal 계산 하기 위한 부분
    //각 점에 대해 for 반복
    for (int i = 0; i < out_vertices.size(); i++)
    {
        std::vector<int> tempList; tempList.clear();
        //각 face에 대해 for반복
        for (int j = 0; j < out_faces.size(); j++)
        {
            //각 face를 이루는 3점에 대해서
            for (int k = 0; k < 3; k++)
            {
                //각 점 i와 각 삼각형에 속한 3점 k와 비교 검사
                //만 약 두 값이 같다면 해당 삼각형은 i점을 포함하는 것
                if (i == out_faces[j][k])
                {
                    //해당 face의 index를 저장
                    tempList.push_back(j);
                }
            }
        }
        triList.push_back(tempList);
    }

    //각 점에서 인접한 삼각형의 normal을 이용해 point normal 계산
    //각 점에 대해 for
    for (int i = 0; i < triList.size(); i++)
    {
        glm::vec3 avgNormal(0, 0, 0);
        // 각 점이 속한 face list에 대해서 for
        for (int j = 0; j < triList[i].size(); j++)
        {
            //미리 계산해둔 face normal을 이용해 평균 계산
            avgNormal += triNormals[triList[i][j]];
        }
        //인접한 삼각형 수 만큼 나눠주기
        avgNormal /= triList[i].size();
        //계산된 결과 결과에 추가
        out_normals.push_back(avgNormal);
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

void MyMouseWheel(int button, int dir, int x, int y)
{
    if (dir > 0)
    {
        //zoom in
        g_fDistance *= 0.9;
    }
    else {
        //zoom out
        g_fDistance *= 1.1;
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
    GLfloat LightPosition[] = { 0.0, 5.0, 0.0, 1.0 };
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
    for (int i = 0; i < faces.size(); i++) {
        for (int j = 0; j < 3; j++)
        {
            glm::vec3 p = vertices[faces[i][j]];
            
            if (normals.size() == vertices.size())
            {
                glm::vec3 n = normals[faces[i][j]];
                glNormal3f(n[0], n[1], n[2]);
            }
            glVertex3f(p[0], p[1], p[2]);
        }
    }
    glEnd();
}

void DrawWireSurface(std::vector < glm::vec3 >& vectices,
    std::vector < glm::ivec3 >& faces)
{
    glBegin(GL_LINES);
    for (int i = 0; i < faces.size(); i++) {
        
        for (int j = 0; j < 2; j++)
        {
            glm::vec3 p1 = vertices[faces[i][j]];
            glm::vec3 p2 = vertices[faces[i][j+1]];

            glVertex3f(p1[0], p1[1], p1[2]);
            glVertex3f(p2[0], p2[1], p2[2]);
        }
        glm::vec3 p1 = vertices[faces[i][0]];
        glm::vec3 p2 = vertices[faces[i][2]];

        glVertex3f(p1[0], p1[1], p1[2]);
        glVertex3f(p2[0], p2[1], p2[2]);
    }

    glEnd();
}


void render(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glTranslatef(0.0f, 0.0f, g_fDistance);
    glRotatef(-g_fSpinY, 1.0f, 0.0f, 0.0f);
    glRotatef(-g_fSpinX, 0.0f, 1.0f, 0.0f);
    glScalef(5, 5, 5);
    //Draw here
   // glutSolidTeapot(0.1);
    DrawSurface(vertices, normals, faces);
    //DrawWireSurface(vertices, faces);
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
    glutCreateWindow("ID name");
    init();

    //LoadObj("Data/bunny/bunny.obj", vertices, faces, uvs, normals);
    LoadPly("Data/bunny/bun_zipper_res3.ply", vertices, faces, normals);

    glutDisplayFunc(render);
    glutReshapeFunc(MyReshape);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMotion);
    glutMouseWheelFunc(MyMouseWheel);

    glutMainLoop();
}