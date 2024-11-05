#include <windows.h>
#include <stdio.h>
#include <gl/glut.h>
#include <gl/glu.h>

GLuint texture;
GLfloat angle = 0.0;

GLuint LoadTexture( const char * filename, int width, int height ) {
     GLuint texture;
     unsigned char * data;
     FILE * file;
 
     file = fopen( filename, "rb" );
     if ( file == NULL ) return 0;
     data = (unsigned char *)malloc( width * height * 3 );
     fread( data, width * height * 3, 1, file );
     fclose( file );
 
     glGenTextures( 1, &texture );
     glBindTexture( GL_TEXTURE_2D, texture );
     glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
 
     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR ); 
     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
     glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
 
     gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data );
     free( data );
     return texture;
 }

void MK_Cube () {
     texture = LoadTexture("Data/Earth.bmp", 256, 256 );
     glEnable( GL_TEXTURE_2D );
     glEnable(GL_TEXTURE_GEN_S);
     glEnable(GL_TEXTURE_GEN_T);

     glBindTexture( GL_TEXTURE_2D, texture );
     glRotatef( angle, 1.0f, 1.0f, 1.0f );
     glutSolidCube(2.0);
 }

void MyDisplay () {
     glClearColor (1.0,1.0,1.0,1.0);
     glClear (GL_COLOR_BUFFER_BIT);
     glLoadIdentity();
     gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

     MK_Cube( );
     glDeleteTextures( 1, &texture );
     glutSwapBuffers();
     angle ++;
}

void MyReshape (int w, int h) {
     glViewport (0, 0, (GLsizei)w, (GLsizei)h);
     glMatrixMode (GL_PROJECTION);
     glLoadIdentity ();

     gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0 );
     glMatrixMode (GL_MODELVIEW);
}

int main (int argc, char **argv) {
     glutInit (&argc, argv);
     glutInitDisplayMode (GLUT_DOUBLE);
     glutInitWindowSize (500, 500);
     glutInitWindowPosition (100, 100);
     glutCreateWindow ("Texture Mapping using Cube");
     glutDisplayFunc (MyDisplay);
     glutIdleFunc (MyDisplay);
     glutReshapeFunc (MyReshape);

     glutMainLoop ( );
     return 0;
}