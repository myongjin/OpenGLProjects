#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "glaux.h"

#define TEXTURE_NUM 1

GLuint texture[TEXTURE_NUM];
char texture_name[TEXTURE_NUM][20] = {"Data/Background.bmp"};

float g_fDistance = -4.5f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

void MyReshape(int w, int h) {
     glViewport(0,0,w,h);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();

     gluPerspective(40.0, (GLfloat)w/(GLfloat)h, 1.0, 100.0);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();

     gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void MyDisplay( ){
     glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
     glMatrixMode( GL_MODELVIEW );
     glLoadIdentity();

	 glClearColor(0.5, 0.5, 0.5, 0.5);

     glTranslatef( 0.0f, 0.0f, g_fDistance );
     glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
     glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

     glBindTexture(GL_TEXTURE_2D, texture[0]);

     glBegin(GL_QUADS);
          glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
          glTexCoord2f(3.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
          glTexCoord2f(3.0f, 3.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
          glTexCoord2f(0.0f, 3.0f); glVertex3f(-1.0f,  1.0f,  1.0f);

          glTexCoord2f(3.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
          glTexCoord2f(3.0f, 3.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
          glTexCoord2f(0.0f, 3.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
          glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);

          glTexCoord2f(0.0f, 3.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
          glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
          glTexCoord2f(3.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
          glTexCoord2f(3.0f, 3.0f); glVertex3f( 1.0f,  1.0f, -1.0f);

          glTexCoord2f(3.0f, 3.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
          glTexCoord2f(0.0f, 3.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
          glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
          glTexCoord2f(3.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);

          glTexCoord2f(3.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
          glTexCoord2f(3.0f, 3.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
          glTexCoord2f(0.0f, 3.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
          glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);

          glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
          glTexCoord2f(3.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
          glTexCoord2f(3.0f, 3.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
          glTexCoord2f(0.0f, 3.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
     glEnd();

     glutSwapBuffers();
}

AUX_RGBImageRec * LoadBMP(char *Filename) {
     FILE * File = NULL;
     if(!Filename) return NULL;
     File = fopen(Filename, "r");
     if(File) {
          fclose(File);
          return auxDIBImageLoad(Filename);
     }
     return NULL;
}

int LoadGLTextures() {
     int Status = TRUE, i;
     AUX_RGBImageRec *TextureImage;

     for(i=0; i<TEXTURE_NUM; i++) {
          TextureImage = NULL;
          if (TextureImage = LoadBMP(texture_name[i])) {
               glGenTextures(1, &texture[i]);
               glBindTexture(GL_TEXTURE_2D, texture[i]);
               glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
               glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
               glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
               glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
               glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX,
                              TextureImage->sizeY, 0,
                              GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
          } else {
               MessageBox(NULL,"Image file has a error !",texture_name[i],MB_ICONERROR);
               Status=false;
          }

          if (TextureImage) {
               if (TextureImage->data) free(TextureImage->data);
               free(TextureImage);
          }
   }
   return Status;
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

     if( bMousing ) {
          g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
          g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
     }

     ptLastMousePosit.x = ptCurrentMousePosit.x;
     ptLastMousePosit.y = ptCurrentMousePosit.y;

     glutPostRedisplay();
}

void main(int argc, char ** argv) {
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
     glutInitWindowPosition(100, 100);
     glutInitWindowSize(800, 600);
     glutCreateWindow("Texture Mapping with WRAP");
     LoadGLTextures();
     glutDisplayFunc(MyDisplay);
     glutReshapeFunc(MyReshape);
     glutMouseFunc(MyMouse);
     glutMotionFunc(MyMotion);

     glEnable(GL_TEXTURE_2D);
     glShadeModel(GL_SMOOTH);
     glClearDepth(1.0);
     glEnable(GL_DEPTH_TEST);
     glDepthFunc(GL_LEQUAL);
     glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

     glutMainLoop( );
}