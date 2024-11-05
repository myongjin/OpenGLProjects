#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <gl/glew.h>
#include <glaux.h>
#include <gl/glut.h>

#pragma warning(suppress : 4996)

GLfloat teapotloc = -0.5;                // Teapot�� ��ġ
GLint animate = 0;                      // Animation ���� ����
GLuint vertexshader, fragmentshader, shaderprogram; // shaders

GLubyte woodtexture[256][256][3];
GLuint texNames[1];                     // Texture buffer
GLuint istex;                             // Blend parameter for texturing
GLuint islight;
GLint texturing = 1;                     // Texturing�� On/Off
GLint lighting = 1;                       // Lighting�� On/Off

// Lighting Fragment Shader�� ���� ������ �Ű� �������� ������ ���� ������
GLuint light0dirn, light0color, light1posn, light1color, ambient, diffuse, specular, shininess;
const int DEMO = 5;

#include "shaders.h"
#include "geometry3.h"

float g_fDistance = -1.5f;
float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

// Modelview�� ���� ���͸� ��ȯ�� �� �ִ� ���ο� ����� ��ȯ ���
void transformvec (const GLfloat input[4], GLfloat output[4]) {
     GLfloat modelview[16];                            // ��(Column) �켱������ ���
     glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
  
     for (int i = 0 ; i < 4 ; i++) {
          output[i] = 0;
          for (int j = 0 ; j < 4 ; j++)
                output[i] += modelview[4*j+i] * input[j];
     }
}

void MyDisplay() {
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glMatrixMode( GL_MODELVIEW );
     glLoadIdentity();

     glTranslatef( 0.0f, 0.0f, g_fDistance );
     glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
     glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

     // Teapot�� �����ϰ� Light�� ����.
     glUniform1i(islight, lighting);
     glUniform1i(istex, texturing);
     // geometry3.h�� �ִ� �Լ��μ� Texture�� ������ �׸��� ���� ���̴�.
     drawtexture(FLOOR, texNames[0]);
     glUniform1i(istex, 0);

     glMatrixMode(GL_MODELVIEW);
     {
          const GLfloat one[] = {1, 1, 1, 1};
          const GLfloat medium[] = {0.5, 0.5, 0.5, 1};
          const GLfloat Small[] = {0.2, 0.2, 0.2, 1};
          const GLfloat high[] = {100};
          const GLfloat zero[] = {0.0, 0.0, 0.0, 1.0};
          const GLfloat light_specular[] = {1, 0.5, 0, 1};
          const GLfloat light_specular1[] = {0, 0.5, 1, 1};
          const GLfloat light_direction[] = {0.5, 0, 0, 0};
          const GLfloat light_position1[] = {-0.5, 0.5, 0.5, 1};

          GLfloat light0[4], light1[4];

          transformvec(light_direction, light0);
          transformvec(light_position1, light1);

          glUniform3fv(light0dirn, 1, light0); 
          glUniform4fv(light0color, 1, light_specular); 
          glUniform4fv(light1posn, 1, light1);
          glUniform4fv(light1color, 1, light_specular1);

          glUniform4fv(ambient, 1, Small);
          glUniform4fv(diffuse, 1, medium);
          glUniform4fv(specular, 1, one);
          glUniform1fv(shininess, 1, high);

          if (DEMO > 4)
               glUniform1i(islight, lighting); // Teapot�� ���� ����(Lighting)�� �Ҵ�.
     }

     glColor3f(0.0, 1.0, 1.0);
     glPushMatrix();
          glTranslatef(teapotloc, 0.0, 0.0);

          glTranslatef(0.0, 0.16, 0.1);
          glutSolidTeapot(0.2);

          glTranslatef(0.0, -0.16, -0.1);
          glTranslatef(0.4, 0.2, -0.3);
          glutSolidSphere(0.2, 50, 50);

          glTranslatef(-0.4, -0.2, 0.3);
          glTranslatef(0.8, 0.2, 0.3);
          glutSolidSphere(0.2, 50, 50);

          glUniform1i(islight, 0);             // ����(Lighting)�� ����.
     glPopMatrix();

     glutSwapBuffers();
     glFlush ();
}

void animation_L() {
     teapotloc = teapotloc - 0.0025;
     if (teapotloc < -0.5) teapotloc = 0.5;

     glutPostRedisplay();
}

void animation_R() {
     teapotloc = teapotloc + 0.0025;
     if (teapotloc > 0.5) teapotloc = -0.5;

     glutPostRedisplay();
}

void MyMouse(int button, int state, int x, int y) {
     switch (button) {
          case GLUT_LEFT_BUTTON:
               if (state == GLUT_DOWN) {
                    ptLastMousePosit.x = ptCurrentMousePosit.x = x;
                    ptLastMousePosit.y = ptCurrentMousePosit.y = y;
                    bMousing = true;
               } else
                    bMousing = false;
                    break;
          case GLUT_MIDDLE_BUTTON:
               animate = !animate ;
               if (animate) glutIdleFunc(animation_L) ;
               else glutIdleFunc(NULL) ;
               break;
          case GLUT_RIGHT_BUTTON:
               animate = !animate ;
               if (animate) glutIdleFunc(animation_R) ;
               else glutIdleFunc(NULL) ;
               break;
          default:
               break;
     }

    glutPostRedisplay();
}

void MyMotion(int x, int y) {
     ptCurrentMousePosit.x = x;
     ptCurrentMousePosit.y = y;

     if( bMousing )
     {
          g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
          g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
     }

     ptLastMousePosit.x = ptCurrentMousePosit.x;
     ptLastMousePosit.y = ptCurrentMousePosit.y;

     glutPostRedisplay();
}

void MyKeyboard (unsigned char key, int x, int y) {
     switch (key) {
          case 'p':                       // Animation�� ���߰ų� ������ϱ� ���ؼ�
               animate = !animate;
               if (animate) glutIdleFunc(animation_R);
               else glutIdleFunc(NULL);
               break;
          case 't':                        // Texturing�� �����ϰų� ���� �ʰų� �ϱ� ���ؼ�
               texturing = !texturing;
               break;
          case 's':                        // Shading�� �����ϰų� ���� �ʰų� �ϱ� ���ؼ�
               lighting = !lighting;
               break;
          default:
               break;
     }

     glutPostRedisplay();
}

void MyReshape(int w, int h) {
     glViewport (0, 0, (GLsizei) w, (GLsizei) h);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();

     gluPerspective(30.0, (GLdouble)w/(GLdouble)h, 1.0, 10.0) ;
     gluLookAt(0.0, 1.2, 1.0, 0.0, 0.8, 0.0, 0.0, 1.0, 0.0) ;
}

void MyInit () {
     glClearColor (0.0, 0.0, 0.0, 0.0);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();

     glMatrixMode(GL_MODELVIEW) ;
     glLoadIdentity();

     vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl");
     fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl");
     shaderprogram = initprogram(vertexshader, fragmentshader);

     // ����(Lighting)�� ���� Shader �Ű� ���� Mapping�� �ùٸ��� �����Ѵ�.
     islight = glGetUniformLocation(shaderprogram, "islight"); 
     light0dirn = glGetUniformLocation(shaderprogram, "light0dirn");
     light0color = glGetUniformLocation(shaderprogram, "light0color");
     light1posn = glGetUniformLocation(shaderprogram, "light1posn");
     light1color = glGetUniformLocation(shaderprogram, "light1color"); 
     ambient = glGetUniformLocation(shaderprogram, "ambient");
     diffuse = glGetUniformLocation(shaderprogram, "diffuse");
     specular = glGetUniformLocation(shaderprogram, "specular");
     shininess = glGetUniformLocation(shaderprogram, "shininess");

     // ���(Scene)�� ���� Geometry�� �����Ѵ�.
     glGenBuffers(numperobj*numobjects+ncolors+1, buffers); 

     // geometry3.h�� �ִ� �Լ��μ� Texture ����(*.ppm)�� �а�,
     // Texture ��ǥ���� ���� ����(Buffer)�� �����Ѵ�.
     inittexture("Data/wood.ppm", shaderprogram);

     // geometry3.h�� �ִ� �Լ��μ� ��ü(Object)�� �ʱ�ȭ�Ѵ�.
     initobject(FLOOR, (GLfloat *) floorverts, sizeof(floorverts), (GLfloat *) floorcol, 
               sizeof (floorcol), (GLubyte *) floorinds, sizeof (floorinds), GL_POLYGON);

     glEnable(GL_DEPTH_TEST);
     glDepthFunc (GL_LESS);
}

int main(int argc, char** argv) {
     glutInit(&argc, argv);
     glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
     glutInitWindowSize (800, 600);
     glutInitWindowPosition (100, 100);
     glutCreateWindow ("Phong Shading with Fragment Shaders");
     glewInit();
     MyInit();
     glutDisplayFunc(MyDisplay);
     glutReshapeFunc(MyReshape);
     glutKeyboardFunc(MyKeyboard);
     glutMouseFunc(MyMouse);
     glutMotionFunc(MyMotion);

     glutMainLoop();
     return 0;  
}