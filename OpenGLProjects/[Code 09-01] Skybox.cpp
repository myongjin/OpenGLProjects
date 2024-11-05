#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <glaux.h>

float g_fDistance = -2.5f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;
float eyex=0.0, eyey=50.0, eyez=100.0, fovy=40.0;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

GLuint tex[7];   // Texture Mapping을 하기 위한 Texture 이미지의 개수를 위한 배열 변수

AUX_RGBImageRec *LoadBMP(char *Filename) {  // Bitmap 이미지를 호출한다.
     FILE *File=NULL;

     if (!Filename) return NULL;
     File=fopen(Filename,"r");
     if (File) {
          fclose(File);
          return auxDIBImageLoad(Filename);
     }

     return NULL;
}

void LoadGLTextures( ) {  // Bitmap 이미지 7개를 호출하여 Texture 이미지로 변환한다.
     AUX_RGBImageRec *texRec[7];
     memset(texRec, 0, sizeof(void *)*7);                                                  

     if((texRec[0]=LoadBMP("Data/skybox1_posz.bmp")) &&
       (texRec[1]=LoadBMP("Data/skybox1_negz.bmp")) &&
       (texRec[2]=LoadBMP("Data/skybox1_posx.bmp")) &&
       (texRec[3]=LoadBMP("Data/skybox1_negx.bmp")) &&
       (texRec[4]=LoadBMP("Data/skybox1_posy.bmp")) &&
       (texRec[5]=LoadBMP("Data/skybox1_negy.bmp")) &&
       (texRec[6]=LoadBMP("Data/Lawn2.bmp")) ) {

          for(int i=0; i<7; i++) {
               glGenTextures(1, &tex[i]); 
               glBindTexture(GL_TEXTURE_2D, tex[i]);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
               glTexImage2D(GL_TEXTURE_2D, 0, 3, texRec[i]->sizeX, texRec[i]->sizeY, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, texRec[i]->data); 
          }
     }

     for(int i=0; i<7; i++) {                                                              
          if(texRec[i]) {
               if(texRec[i]->data) free(texRec[i]->data);
               free(texRec[i]);
          } 
     }

     glEnable(GL_TEXTURE_2D);
     glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void MyDisplay( ) {
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glMatrixMode( GL_MODELVIEW );
     glLoadIdentity();

     glTranslatef( 0.0f, 0.0f, g_fDistance );
     glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
     glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

     gluLookAt(eyex, eyey, eyez, 0.0, 50.0, 0.0, 0.0, 1.0, 0.0);  // 카메라의 위치를 제어한다.

     float r = 1000.0f;

     // Skybox의 앞면을 Rendering 한다.
     glBindTexture(GL_TEXTURE_2D, tex[0]);
     glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( -r, -r/2, -r );
          glTexCoord2f(1, 0); glVertex3f(  r, -r/2, -r );
          glTexCoord2f(1, 1); glVertex3f(  r, r, -r );
          glTexCoord2f(0, 1); glVertex3f( -r, r, -r );
     glEnd();

     // Skybox의 뒷면을 Rendering 한다.
     glBindTexture(GL_TEXTURE_2D, tex[1]);
     glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f(  r, -r/2,  r );
          glTexCoord2f(1, 0); glVertex3f( -r, -r/2,  r );
          glTexCoord2f(1, 1); glVertex3f( -r, r,  r );
          glTexCoord2f(0, 1); glVertex3f(  r, r,  r );
     glEnd();

     // Skybox의 오른쪽 면을 Rendering 한다.
     glBindTexture(GL_TEXTURE_2D, tex[2]);
     glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f(  r, -r/2, -r );
          glTexCoord2f(1, 0); glVertex3f(  r, -r/2,  r );
          glTexCoord2f(1, 1); glVertex3f(  r, r,  r );
          glTexCoord2f(0, 1); glVertex3f(  r, r, -r );
     glEnd();

     // Skybox의 왼쪽 면을 Rendering 한다.
     glBindTexture(GL_TEXTURE_2D, tex[3]);
     glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( -r, -r/2,  r );
          glTexCoord2f(1, 0); glVertex3f( -r, -r/2, -r );
          glTexCoord2f(1, 1); glVertex3f( -r, r, -r );
          glTexCoord2f(0, 1); glVertex3f( -r, r,  r );
     glEnd();

     // Skybox의 윗면을 Rendering 한다.
     glBindTexture(GL_TEXTURE_2D, tex[4]);
     glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( -r,  r, -r );
          glTexCoord2f(1, 0); glVertex3f(  r,  r, -r );
          glTexCoord2f(1, 1); glVertex3f(  r,  r,  r );
          glTexCoord2f(0, 1); glVertex3f( -r,  r,  r );
     glEnd();

     // Skybox의 아랫면을 Rendering 한다.
     glBindTexture(GL_TEXTURE_2D, tex[5]);
     glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( -r, -r/2,  r );
          glTexCoord2f(1, 0); glVertex3f(  r, -r/2,  r );
          glTexCoord2f(1, 1); glVertex3f(  r, -r/2, -r );
          glTexCoord2f(0, 1); glVertex3f( -r, -r/2, -r );
     glEnd();

     // Skybox의 지표면을 Rendering 한다.
     glBindTexture(GL_TEXTURE_2D, tex[6]);
     glBegin(GL_QUADS);
          glTexCoord2f(0, 0); glVertex3f( -r, 0.1f,  r );
          glTexCoord2f(1, 0); glVertex3f(  r, 0.1f,  r );
          glTexCoord2f(1, 1); glVertex3f(  r, 0.1f, -r );
          glTexCoord2f(0, 1); glVertex3f( -r, 0.1f, -r );
     glEnd();

     glutSwapBuffers();
}

void MySpecial(int key, int x, int y) {
     if(key == GLUT_KEY_UP) {
          g_fDistance += 2.0f;
     } else if(key == GLUT_KEY_DOWN) {
          g_fDistance -= 2.0f;
     } else if(key == GLUT_KEY_LEFT) {
          eyex += 1.0f;
     } else if(key == GLUT_KEY_RIGHT) {
          eyex -= 1.0f;
     }

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

     if( bMousing )
     {
          g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
          g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
     }

     ptLastMousePosit.x = ptCurrentMousePosit.x;
     ptLastMousePosit.y = ptCurrentMousePosit.y;

     glutPostRedisplay();
}

void MyReshape(int w, int h) {
     glViewport(0,0,w,h);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();

     gluPerspective(fovy, (GLfloat)w/(GLfloat)h, 1.0, 2000.0);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
}

void main(int argc, char **argv) {
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
     glutInitWindowPosition(100, 100);
     glutInitWindowSize(800, 600);
     glutCreateWindow("Skybox");
     LoadGLTextures();
     glutDisplayFunc(MyDisplay);
     glutReshapeFunc(MyReshape);
     glutMouseFunc(MyMouse);
     glutMotionFunc(MyMotion);
     glutSpecialFunc(MySpecial);

     glutMainLoop( );
}