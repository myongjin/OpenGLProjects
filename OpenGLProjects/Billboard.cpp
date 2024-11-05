#include <windows.h>
#include <mmsystem.h>
#include <glaux.h>
#include <gl/glut.h>
#include <gl/glu.h>
#include "matrix4x4f.h"
#include "vector3f.h"
#include "tga.h"

GLuint g_treeTextureID = -1;
GLuint g_grassTextureID = -1;
bool   g_bBillboard = true;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

float g_fDistance = 0.0f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;

void LoadTexture( void ) {
    tgaImageFile tgaImage;
    tgaImage.load( "Data/maple_tree.tga" );

    glGenTextures( 1, &g_treeTextureID );
    glBindTexture( GL_TEXTURE_2D, g_treeTextureID );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, tgaImage.m_texFormat, 
                  tgaImage.m_nImageWidth, tgaImage.m_nImageHeight, 
                  0, tgaImage.m_texFormat, GL_UNSIGNED_BYTE, 
                  tgaImage.m_nImageData );

    AUX_RGBImageRec *pTextureImage = auxDIBImageLoad( "Data/Lawn.bmp" );

    if( pTextureImage != NULL ) {
        glGenTextures( 1, &g_grassTextureID );

        glBindTexture( GL_TEXTURE_2D, g_grassTextureID );

        glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );

        glTexImage2D( GL_TEXTURE_2D, 0, 3, pTextureImage->sizeX,
                      pTextureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
                      pTextureImage->data );
    }

    if( pTextureImage != NULL ) {
        if( pTextureImage->data != NULL )
            free( pTextureImage->data );

        free( pTextureImage );
    }
}

void buildAxisAlignedBBMatrix( float m[16], float x, float y, float z ) {
    float pi    = 3.141592654f;
    float theta = -180 * atan2f(m[8], m[10]) / pi;
    float d     = x*x + y*y + z*z;
    float ct    = cosf(DEGTORAD(theta));
    float st    = sinf(DEGTORAD(theta));

    // Normalize
    if( d > 0 ) {
	    d = 1/d;
	    x *= d;
	    y *= d;
	    z *= d;
    }

    // Clear out the view matrix passed in
    m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;
    m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;
    m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;

    //---------------------------------------------
    // R = uu' + cos(theta)*(I-uu') + sin(theta)*S
    //
    // S =  0  -z   y    u' = (x, y, z)
    //	    z   0  -x
    //	   -y   x   0
    //---------------------------------------------

     m[0] = x*x + ct*(1-x*x) + st*0;
     m[4] = x*y + ct*(0-x*y) + st*-z;
     m[8] = x*z + ct*(0-x*z) + st*y;

     m[1] = y*x + ct*(0-y*x) + st*z;
     m[5] = y*y + ct*(1-y*y) + st*0;
     m[9] = y*z + ct*(0-y*z) + st*-x;

     m[2] = z*x + ct*(0-z*x) + st*-y;
     m[6] = z*y + ct*(0-z*y) + st*x;
     m[10]= z*z + ct*(1-z*z) + st*0;
}

void DrawGLScene( void ) {
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	gluLookAt(0.0, 3.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glTranslatef( 0.0f, 0.0f, g_fDistance );
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

	glRotatef(-180.0, 0.0, 1.0, 0.0);
    glBindTexture( GL_TEXTURE_2D, g_grassTextureID );
    glBegin( GL_POLYGON ); 
	{        
        glTexCoord2f(0.0f, 0.0f);	glVertex3f(-5.0f, -1.0f, -3.0f);
		glTexCoord2f(1.0f, 0.0f);	glVertex3f( 5.0f, -1.0f, -3.0f);
        glTexCoord2f(1.0f, 1.0f);	glVertex3f( 5.0f, -1.0f, 3.0f);
        glTexCoord2f(0.0f, 1.0f);	glVertex3f(-5.0f, -1.0f, 3.0f);
    }
    glEnd();
	glRotatef(180.0, 0.0, 1.0, 0.0);

	if( g_bBillboard == true ) {
        float mat[16];

        glGetFloatv( GL_MODELVIEW_MATRIX, mat );

        // Do billboard rotation about the y axis
        buildAxisAlignedBBMatrix( mat, 0.0f, 1.0f, 0.0f ); 

        glMultMatrixf( mat );
    }

    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glEnable( GL_ALPHA_TEST );
    glAlphaFunc( GL_GREATER, 0 );
	
    glBindTexture( GL_TEXTURE_2D, g_treeTextureID );
    glBegin( GL_POLYGON ); {
        glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);	glVertex3f(1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 1.0f, 0.0f);
    }
    glEnd();

    glDisable( GL_BLEND );
    glDisable( GL_ALPHA_TEST );

	glutSwapBuffers();
}

void InitGL( void ) {
	glClearColor( 0.35f, 0.53f, 0.7f, 1.0f );
    glEnable( GL_TEXTURE_2D );
    glEnable( GL_DEPTH_TEST );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

    LoadTexture();
}

void MySpecial(int key, int x, int y) {
	if(key == GLUT_KEY_PAGE_UP) {
		g_fDistance -= 1.1f;
	} else if(key == GLUT_KEY_PAGE_DOWN) {
		g_fDistance += 1.1f;
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
    gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void main(int argc, char **argv){
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Billboard");
	InitGL();	
    glutDisplayFunc(DrawGLScene);
    glutReshapeFunc(MyReshape);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);
	glutSpecialFunc(MySpecial);
    glutMainLoop( );
}