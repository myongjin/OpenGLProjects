#include <Windows.h>
#include <math.h>
#include <stdio.h>
#include <gl/glut.h>

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

float g_fDistance = -7.0f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;

static	bool antialiasFlag = GL_FALSE;

void initgfx() {
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glShadeModel( GL_FLAT );
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	/* make the point size large so that the points don't disappear when antialiasing is enabled */
	glLineWidth( 10.5 );

	glEnable( GL_DEPTH_TEST );
}

void Antialiasing() {
	antialiasFlag = !antialiasFlag;

	if ( antialiasFlag == GL_TRUE ) {
		glEnable( GL_POINT_SMOOTH );
		glEnable( GL_LINE_SMOOTH );
	} else {
		glDisable( GL_POINT_SMOOTH );
		glDisable( GL_LINE_SMOOTH );
	}
}

void MySpecial(int key, int x, int y) {
	if(key == GLUT_KEY_UP) {
		g_fDistance -= 0.5f;
	} else if(key == GLUT_KEY_DOWN) {
		g_fDistance += 0.5f;
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
			if (state == GLUT_DOWN)	Antialiasing();
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
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	gluPerspective( 45.0, w/h, 0.1, 100 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void drawScene() {	
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glTranslatef( 0.0f, 0.0f, g_fDistance );
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

	glRotatef(20.0, 1.0, 1.0, 0.0);
	glColor4f(0.0, 1.0, 0.0, 1.0);
	glutWireCube(1.5);

	glColor4f(1.0, 1.0, 0.0, 1.0);
	glPointSize(20.0);
	glBegin(GL_POINTS);
		glVertex3f(0, 0, 0);
	glEnd();

	glColor4f(1.0, 0.0, 1.0, 1.0);
	glScalef( 2.0f, 2.0f, 2.0f );
	glutWireIcosahedron();

	glutSwapBuffers();
}

void main( int argc, char **argv ) {
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition(10, 10);
	glutInitWindowSize( 800, 600 );
	glutCreateWindow("Anti-Aliasing using Point Smooth and Line Smooth");

	initgfx();

	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);
	glutSpecialFunc(MySpecial);
	glutReshapeFunc(MyReshape);
	glutDisplayFunc(drawScene); 

	glutMainLoop();
}