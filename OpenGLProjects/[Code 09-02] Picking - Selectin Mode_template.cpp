#include <Windows.h>
#include <stdio.h>
#include <GL/glut.h>
#include <GL/glu.h>

//for drawing fonts
#include "bitmap_fonts.h"

#include <vector>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

static POINT	ptLastMousePosit;
static POINT	ptCurrentMousePosit;
static bool		bMousing;

static char message[255];

int    g_nWindowWidth;
int    g_nWindowHeight;

float g_fDistance = -5.0f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;


glm::vec4 ConvertVector(glm::vec4 p)
{
	glm::vec4 convP = p;

	return convP;
}

void DrawUI()
{
	glDisable(GL_LIGHTING);

	//Draw box UI
	glPushName(106);
	glColor4f(0, 0, 0.5, 0.5);
	glBegin(GL_QUADS);
	glm::vec4 convP = ConvertVector(glm::vec4(-1, -1, 0, 1));
	glVertex4f(convP[0], convP[1], convP[2], 1.f);

	convP = ConvertVector(glm::vec4(1, -1, 0, 1));
	glVertex4f(convP[0], convP[1], convP[2], 1.f);

	convP = ConvertVector(glm::vec4(1, -0.5, 0, 1));
	glVertex4f(convP[0], convP[1], convP[2], 1.f);

	convP = ConvertVector(glm::vec4(-1, -0.5, 0, 1));
	glVertex4f(convP[0], convP[1], convP[2], 1.f);

	glEnd();
	glPopName();

	//Draw font
	beginRenderText(g_nWindowWidth, g_nWindowHeight);
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		float u, v;

		renderText(u, v, BITMAP_FONT_TYPE_HELVETICA_18, message);
	}
	endRenderText();

	glEnable(GL_LIGHTING);
}

void DrawGLScene()	{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

    glTranslatef( 0.0f, 0.0f, g_fDistance );
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float mat[16];
	glm::f32 values[16];


	glInitNames();
	glEnable(GL_LIGHTING);

	glPushName(100);
	glPushMatrix();
		glColor3f(1.0f, 0.0f, 0.0f);
		glutWireTeapot(0.3);
	glPopMatrix();
	glPopName();

	glPushName(101);
	glPushMatrix();
		glColor3f(0.0f, 1.0f, 0.0f);
		glTranslatef(-1.0f, -1.0f, 0.0f);
		glutSolidSphere(0.3, 30, 30);
	glPopMatrix();
	glPopName();

	glPushName(102);	
	glPushMatrix();	
		glColor3f(0.0f, 0.0f, 1.0f);
		glTranslatef(1.0f, 1.0f, 0.0f);
		glutSolidSphere(0.3, 30, 30);
	glPopMatrix();
	glPopName();

	glPushName(103);
	glPushMatrix();
		glColor3f(1.0f, 1.0f, 0.0f);
		glTranslatef(-1.0f, 1.0f, 0.0f);
		glutSolidSphere(0.3, 30, 30);
	glPopMatrix();
	glPopName();

	glPushName(104);
	glPushMatrix();
		glColor3f(0.0f, 1.0f, 1.0f);
		glTranslatef(1.0f, -1.0f, 0.0f);
		glutSolidSphere(0.3, 30, 30);
	glPopMatrix();
	glPopName();

	glDisable(GL_LIGHTING);

	glPushName(105);
	glColor3f(0.7f, 0.7f, 1.0f);
	glBegin(GL_LINES);
		glVertex2f(-1.0f, -1.0f);
		glVertex2f(1.0f, -1.0f);

		glVertex2f(1.0f, -1.0f);
		glVertex2f(1.0f, 1.0f);

		glVertex2f(1.0f, 1.0f);
		glVertex2f(-1.0f, 1.0f);
	
		glVertex2f(-1.0f, 1.0f);
		glVertex2f(-1.0f, -1.0f);

		glVertex2f(0.0f, 1.5f);
		glVertex2f(0.0f, -1.5f);

		glVertex2f(1.5f, 0.0f);
		glVertex2f(-1.5f, 0.0f);

		glVertex2f(1.5f, 1.5f);
		glVertex2f(-1.5f, -1.5f);
	glEnd();
	glPopName();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
		glVertex2f(-1.5f, 1.5f);
		glVertex2f(1.5f, -1.5f);
	glEnd();


	//Draw UI
	DrawUI();

	glutSwapBuffers();
}

void ProcessSelect(GLuint index[64]) {
	switch(index[3]) {
		case 100: MessageBox(NULL, "Red Wire Teapot", "Selection", MB_OK); break;
		case 101: MessageBox(NULL, "Green Solid Sphere", "Selection", MB_OK); break;
		case 102: MessageBox(NULL, "Blue Solid Sphere", "Selection", MB_OK); break;
		case 103: MessageBox(NULL, "Yellow Solid Sphere", "Selection", MB_OK); break;
		case 104: MessageBox(NULL, "Cyan Solid Sphere", "Selection", MB_OK); break;
		case 105: MessageBox(NULL, "Line", "Selection", MB_OK); break;

		default: MessageBox(NULL, "None?", "Selection", MB_OK); break;
	}
}

void SetTextMessage(GLuint index[64])
{
	switch (index[3]) {
		
	case 100: sprintf_s(message, "Red Wire Teapot"); break;
	case 101: sprintf_s(message, "Green Solid Sphere"); break;
	case 102: sprintf_s(message, "Blue Solid Sphere"); break;
	case 103: sprintf_s(message, "Yellow Solid Sphere"); break;
	case 104: sprintf_s(message, "Cyan Solid Sphere"); break;
	case 105: sprintf_s(message, "Line"); break;
	case 106: sprintf_s(message, "UI"); break;

	default: sprintf_s(message, "None"); break;
	}
}

void SelectObjects(GLint x, GLint y) {
	GLuint selectBuff[64];
	GLint hits, viewport[4];

	glSelectBuffer(64, selectBuff);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glRenderMode(GL_SELECT);
		glLoadIdentity();
		gluPickMatrix(x, viewport[3]-y, 2, 2, viewport);

		gluPerspective(45.0f, (GLfloat)g_nWindowWidth / (GLfloat)g_nWindowHeight, 0.1f, 100.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		DrawGLScene();

		hits = glRenderMode(GL_RENDER);
		if (hits > 0)
		{
			//ProcessSelect(selectBuff);
			//SetTextMessage(selectBuff);
		}

		glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void MyMouse(int button, int state, int x, int y) {
   switch (button) {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {
            ptLastMousePosit.x = ptCurrentMousePosit.x = x;
            ptLastMousePosit.y = ptCurrentMousePosit.y = y;
			SelectObjects(ptCurrentMousePosit.x, ptCurrentMousePosit.y);
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

int InitGL() {
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	return TRUE;
}

void MyReshape( int w, int h ) {
	g_nWindowWidth = w;
	g_nWindowHeight = h;

    glViewport( 0, 0, (GLsizei)w, (GLsizei)h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    gluPerspective (45, (GLfloat)w / (GLfloat)h, 0.1, 100.0);
}

int main(int argc, char** argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (640, 480); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("Selection Mode - Picking");
   InitGL();
   glutDisplayFunc( DrawGLScene ); 
   glutReshapeFunc( MyReshape );
   glutMouseFunc(MyMouse);
   glutMotionFunc(MyMotion);

   glutMainLoop();
   return 0;   
}
