#include <windows.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "glaux.h"

GLuint g_textureID = -1;

struct Vertex {
    // GL_T2F_V3F
    float tu, tv;
    float x, y, z;
};

Vertex g_quadVertices[] = {
    { 0.0f, 0.0f, -1.0f,-1.0f, 0.0f },
    { 1.0f, 0.0f,  1.0f,-1.0f, 0.0f },
    { 1.0f, 1.0f,  1.0f, 1.0f, 0.0f },
    { 0.0f, 1.0f, -1.0f, 1.0f, 0.0f }
};

Vertex g_parallelogramVertices[] = {
    { 0.0f,0.0f, -1.0f,-1.0f, 0.0f },
    { 1.0f,0.0f,  1.0f,-1.0f, 0.0f },
    { 1.0f,1.0f,  2.0f, 1.0f, 0.0f },
    { 0.0f,1.0f,  0.0f, 1.0f, 0.0f }
};

Vertex g_triangleVertices[] = {
    { 0.0f,0.0f, -1.0f,-1.0f, 0.0f },
    { 0.5f,1.0f,  0.0f, 1.0f, 0.0f },
    { 1.0f,0.0f,  1.0f,-1.0f, 0.0f }
};

Vertex g_polygonVertices[] = {
    { 0.2f,0.0f, -0.3f,-1.0f, 0.0f },
    { 0.8f,0.0f,  0.3f,-1.0f, 0.0f },
    { 1.0f,0.5f,  1.0f, 0.0f, 0.0f },
    { 0.5f,1.0f,  0.0f, 1.0f, 0.0f },
    { 0.0f,0.5f, -1.0f, 0.0f, 0.0f }
};

void loadTexture(void) {
	AUX_RGBImageRec *pTextureImage = auxDIBImageLoad( "Data/Background.bmp" );

    if( pTextureImage != NULL ) {
        glGenTextures( 1, &g_textureID );

		glBindTexture( GL_TEXTURE_2D, g_textureID );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

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
	
	glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	glTranslatef( -2.0f, 1.0f, -6.0f );
    glBindTexture( GL_TEXTURE_2D, g_textureID );
    glInterleavedArrays( GL_T2F_V3F, 0, g_quadVertices );
    glDrawArrays( GL_QUADS, 0, 4 );
	glTranslatef( 2.0f, -0.0f, 6.0f );

	glTranslatef( 1.0f, 0.0f, -6.0f );
    glBindTexture( GL_TEXTURE_2D, g_textureID );
    glInterleavedArrays( GL_T2F_V3F, 0, g_parallelogramVertices );
    glDrawArrays( GL_QUADS, 0, 4 );
	glTranslatef(-1.0f, 0.0f, 6.0f );

	glTranslatef( -2.0f, -2.2f, -6.0f );
    glBindTexture( GL_TEXTURE_2D, g_textureID );
    glInterleavedArrays( GL_T2F_V3F, 0, g_triangleVertices );
    glDrawArrays( GL_TRIANGLES, 0, 3 );
	glTranslatef( 2.0f, 2.2f, 6.0f );

	glTranslatef( 1.0f, -2.2f, -6.0f );
    glBindTexture( GL_TEXTURE_2D, g_textureID );
    glInterleavedArrays( GL_T2F_V3F, 0, g_polygonVertices );
    glDrawArrays( GL_POLYGON, 0, 5 );
	glTranslatef(-1.0f, 2.2f, 6.0f );

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
    glutCreateWindow("Texture Mapping - Passivity");
	init();
	glutDisplayFunc(render);
    glutReshapeFunc(MyReshape);

    glutMainLoop( );
}