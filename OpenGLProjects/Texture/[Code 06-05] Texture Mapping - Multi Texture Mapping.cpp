#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include "glaux.h"
#include <glext.h>

PFNGLACTIVETEXTUREARBPROC        glActiveTextureARB       = NULL;
PFNGLMULTITEXCOORD2FARBPROC     glMultiTexCoord2fARB     = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB  = NULL;

GLuint g_textureID_0 = -1;
GLuint g_textureID_1 = -1;

float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;
bool g_bModulate = true;

void MyReshape(int w, int h) {
     glViewport(0,0,w,h);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();

     gluPerspective(40.0, (GLfloat)w/(GLfloat)h, 1.0, 100.0);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();

     gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void loadTexture( void ) {
     AUX_RGBImageRec *pTextureImage = auxDIBImageLoad( "path.bmp" );

     if( pTextureImage != NULL ) {
          glGenTextures( 1, &g_textureID_0 );
          glBindTexture(GL_TEXTURE_2D, g_textureID_0);
          glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
          glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
          glTexImage2D( GL_TEXTURE_2D, 0, 3, pTextureImage->sizeX,
                         pTextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         pTextureImage->data );
     }

     if( pTextureImage ) {
          if( pTextureImage->data )
               free( pTextureImage->data );

          free( pTextureImage );
     }

     pTextureImage = auxDIBImageLoad( "lawn.bmp" );

     if( pTextureImage != NULL ) {
          glGenTextures( 1, &g_textureID_1 );
          glBindTexture(GL_TEXTURE_2D, g_textureID_1);
          glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
          glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
          glTexImage2D( GL_TEXTURE_2D, 0, 3, pTextureImage->sizeX,
                         pTextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, 
                         pTextureImage->data );
     }

     if( pTextureImage ) {
          if( pTextureImage->data )
               free( pTextureImage->data );

          free( pTextureImage );
     }
}

void MyInit( ) {
     loadTexture();

     glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
     glEnable(GL_TEXTURE_2D);

     glMatrixMode( GL_PROJECTION );
     glLoadIdentity();
     gluPerspective( 45.0f, 640.0f / 480.0f, 0.1f, 100.0f);

     char *ext = (char*)glGetString( GL_EXTENSIONS );

     if( strstr( ext, "GL_ARB_multitexture" ) == NULL ) {
          MessageBox(NULL,"GL_ARB_multitexture extension was not found",
                    "ERROR",MB_OK|MB_ICONEXCLAMATION);
          return;
     } else {
          glActiveTextureARB       =
         (PFNGLCLIENTACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
          glMultiTexCoord2fARB     =
          (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress("glMultiTexCoord2fARB");
          glClientActiveTextureARB =
          (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glClientActiveTextureARB");

          if( !glActiveTextureARB || !glMultiTexCoord2fARB || !glClientActiveTextureARB ) {
               MessageBox(NULL,"One or more GL_ARB_multitexture functions were notfound","ERROR",MB_OK|MB_ICONEXCLAMATION);
               return;
          }
     }
}

void MyDisplay( ) {
     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

     glMatrixMode( GL_MODELVIEW );
     glLoadIdentity();
     glTranslatef( 0.0f, 0.0f, -4.0f );

     glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
     glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

     glActiveTextureARB( GL_TEXTURE0_ARB );
     glBindTexture( GL_TEXTURE_2D, g_textureID_0 );

     glActiveTextureARB( GL_TEXTURE1_ARB );
     if( g_bModulate == true )
          glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
     else
          glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );

     glBindTexture( GL_TEXTURE_2D, g_textureID_1 );
     glEnable( GL_TEXTURE_2D );

     glBegin( GL_QUADS );
     {
          glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f, 0.0f );
          glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f, 0.0f );
          glVertex3f( -1.5f, -1.0f, 0.0f );

          glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 0.0f, 1.0f );
          glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 0.0f, 1.0f );
          glVertex3f( -1.5f, 1.0f, 0.0f );

          glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f, 1.0f );
          glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f, 1.0f );
          glVertex3f( 1.5f,  1.0f, 0.0f );

          glMultiTexCoord2fARB( GL_TEXTURE0_ARB, 1.0f, 0.0f );
          glMultiTexCoord2fARB( GL_TEXTURE1_ARB, 1.0f, 0.0f );
          glVertex3f( 1.5f, -1.0f, 0.0f );
     }
     glEnd();

     glutSwapBuffers();
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
     glutCreateWindow("Multi Texture Mapping");
     MyInit();
     glutDisplayFunc(MyDisplay);
     glutReshapeFunc(MyReshape);
     glutMouseFunc(MyMouse);
     glutMotionFunc(MyMotion);

     glutMainLoop( );
}