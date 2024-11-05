#include <windows.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include "tga.h"

GLuint g_textureID = -1;

bool g_bBlending = true;
bool g_bSortUsingCullModeTrick = true;

float g_fDistance = -4.5f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

struct Vertex {
    float tu, tv;
    float x, y, z;
};

Vertex g_cubeVertices[] = {
    { 0.0f,0.0f, -1.0f,-1.0f, 1.0f },
    { 1.0f,0.0f,  1.0f,-1.0f, 1.0f },
    { 1.0f,1.0f,  1.0f, 1.0f, 1.0f },
    { 0.0f,1.0f, -1.0f, 1.0f, 1.0f },
   
    { 1.0f,0.0f, -1.0f,-1.0f,-1.0f },
    { 1.0f,1.0f, -1.0f, 1.0f,-1.0f },
    { 0.0f,1.0f,  1.0f, 1.0f,-1.0f },
    { 0.0f,0.0f,  1.0f,-1.0f,-1.0f },
   
    { 0.0f,1.0f, -1.0f, 1.0f,-1.0f },
    { 0.0f,0.0f, -1.0f, 1.0f, 1.0f },
    { 1.0f,0.0f,  1.0f, 1.0f, 1.0f },
    { 1.0f,1.0f,  1.0f, 1.0f,-1.0f },
   
    { 1.0f,1.0f, -1.0f,-1.0f,-1.0f },
    { 0.0f,1.0f,  1.0f,-1.0f,-1.0f },
    { 0.0f,0.0f,  1.0f,-1.0f, 1.0f },
    { 1.0f,0.0f, -1.0f,-1.0f, 1.0f },
   
    { 1.0f,0.0f,  1.0f,-1.0f,-1.0f },
    { 1.0f,1.0f,  1.0f, 1.0f,-1.0f },
    { 0.0f,1.0f,  1.0f, 1.0f, 1.0f },
    { 0.0f,0.0f,  1.0f,-1.0f, 1.0f },
   
    { 0.0f,0.0f, -1.0f,-1.0f,-1.0f },
    { 1.0f,0.0f, -1.0f,-1.0f, 1.0f },
    { 1.0f,1.0f, -1.0f, 1.0f, 1.0f },
    { 0.0f,1.0f, -1.0f, 1.0f,-1.0f }
};

void MyReshape(int w, int h) {	
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 100.0);
}

void MySpecial(int key, int x, int y) {
	if(key == GLUT_KEY_F1)	{
		g_bBlending = !g_bBlending;
	} else if(key == GLUT_KEY_F2) {
		g_bSortUsingCullModeTrick = !g_bSortUsingCullModeTrick;
	} else if(key == GLUT_KEY_UP) {
		g_fDistance -= 0.1f;
	} else if(key == GLUT_KEY_DOWN) {
		g_fDistance += 0.1f;
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

void loadTexture( void )	
{
    tgaImageFile tgaImage;
    tgaImage.load( "Data/radiation_box.tga" );

    glGenTextures( 1, &g_textureID );

    glBindTexture( GL_TEXTURE_2D, g_textureID );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, tgaImage.m_texFormat, 
                  tgaImage.m_nImageWidth, tgaImage.m_nImageHeight, 
                  0, tgaImage.m_texFormat, GL_UNSIGNED_BYTE, 
                  tgaImage.m_nImageData );
}

void init( void ) {
	loadTexture();

	glClearColor( 0.35f, 0.53f, 0.7f, 1.0f );
	glEnable( GL_TEXTURE_2D );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.0f, 640.0f / 480.0f, 0.1f, 100.0f);
}


void render( void ) {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.0f, 0.0f, g_fDistance );
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

	if( g_bBlending == true )
	{
		glDisable( GL_DEPTH_TEST );

        glEnable( GL_BLEND );
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glBindTexture( GL_TEXTURE_2D, g_textureID );

        if( g_bSortUsingCullModeTrick == true )
	    {
            //
            // Use the cull-mode sorting trick for convex non-overlapping 
            // geometry.
            //

            glEnable( GL_CULL_FACE );

            //
            // Render the cube but only render the back-facing polygons.
            //

            glCullFace( GL_FRONT );

            glInterleavedArrays( GL_T2F_V3F, 0, g_cubeVertices );
            glDrawArrays( GL_QUADS, 0, 24 );

            //
            // Render the cube again, but this time we only render the 
            // front-facing polygons.
            //

            glCullFace( GL_BACK );

            glInterleavedArrays( GL_T2F_V3F, 0, g_cubeVertices );
            glDrawArrays( GL_QUADS, 0, 24 );

            glDisable( GL_CULL_FACE );
        }
        else
        {
            //
            // Do no sorting and hope for the best. From certain viewing 
            // positions the cube's sides will appear sorted correctly, but this
            // is typically rare and the cube will not look right most of the 
            // time.
            //

            glInterleavedArrays( GL_T2F_V3F, 0, g_cubeVertices );
            glDrawArrays( GL_QUADS, 0, 24 );
        }
	} else {
        //
        // Render the cube, but do no blending...
        //

		glDisable( GL_BLEND );
		glEnable( GL_DEPTH_TEST );

        glBindTexture( GL_TEXTURE_2D, g_textureID );
        glInterleavedArrays( GL_T2F_V3F, 0, g_cubeVertices );
        glDrawArrays( GL_QUADS, 0, 24 );
	}

	glutSwapBuffers();
}


void main(int argc, char **argv){
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
    glutCreateWindow("Texture Mapping with Alpha");
	init();
	glutDisplayFunc(render);
    glutReshapeFunc(MyReshape);
	glutSpecialFunc(MySpecial);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);

    glutMainLoop( );
}