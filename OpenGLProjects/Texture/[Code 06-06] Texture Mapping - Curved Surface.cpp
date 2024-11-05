#include <windows.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include "glaux.h"

GLuint g_textureID = -1;

void loadTexture(void) {
	AUX_RGBImageRec *pTextureImage = auxDIBImageLoad( "Data/waterfall.bmp" );

    if( pTextureImage != NULL ) {
        glGenTextures( 1, &g_textureID );

		glBindTexture( GL_TEXTURE_2D, g_textureID );

		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_REPEAT );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		glTexImage2D( GL_TEXTURE_2D, 0, 3, pTextureImage->sizeX, pTextureImage->sizeY, 0,
				GL_RGB, GL_UNSIGNED_BYTE, pTextureImage->data );

	}

	if( pTextureImage ) {
		if( pTextureImage->data )
			free( pTextureImage->data );

		free( pTextureImage );
	}
}

void init( void ) {
	loadTexture();

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glEnable( GL_TEXTURE_2D );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
}

void render( void ) {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	GLUquadricObj *qobj;
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, GLU_FILL);
	gluQuadricTexture(qobj, GL_TRUE);
    gluQuadricNormals(qobj, GLU_SMOOTH);

	glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	glTranslatef( -1.3f, 0.0f, -5.0f );
    glBindTexture( GL_TEXTURE_2D, g_textureID );
	glRotatef(-45.0, 1.0, 0.0, 0.0);
    gluSphere(qobj, 1.0, 15, 15);
	glRotatef(45.0, 1.0, 0.0, 0.0);
	glTranslatef( 1.3f, 0.0f, 5.0f );

	glTranslatef( 1.5f, -0.6f, -5.0f );
    glBindTexture( GL_TEXTURE_2D, g_textureID );
	glRotatef(-110.0, 1.0, 0.0, 0.0);
    gluCylinder(qobj, 0.8, 0.8, 1.5, 15, 15);
	glRotatef(110.0, 1.0, 0.0, 0.0);
	glTranslatef(-1.5f, 0.6f, 5.0f );
	
	gluDeleteQuadric(qobj);
	glutSwapBuffers();
}

void MyReshape(int w, int h) {	
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 100.0);
}

void main(int argc, char **argv) {
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
    glutCreateWindow("Texture Mapping of Curved Surface");
	init();
	glutDisplayFunc(render);
    glutReshapeFunc(MyReshape);

    glutMainLoop( );
}