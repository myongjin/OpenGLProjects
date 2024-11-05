#include <stdio.h>
#include "glaux.h"
#include <gl/glut.h>

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

float g_fDistance = -4.0f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;

GLuint	texture[1];

AUX_RGBImageRec *LoadBMP(const char *Filename) {
	FILE *File=NULL;

	if (!Filename) {
		return NULL;
	}

	File=fopen(Filename,"r");

	if (File) {
		fclose(File);
		return auxDIBImageLoad(Filename);
	}

	return NULL;
}

int LoadGLTextures() {
	int Status=FALSE;
	AUX_RGBImageRec *TextureImage[1];
	memset(TextureImage,0,sizeof(void *)*1);

	if (TextureImage[0]=LoadBMP("Data/Lights.bmp")) {
		Status=TRUE;
		glGenTextures(1, &texture[0]);

		glBindTexture(GL_TEXTURE_2D, texture[0]);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
	}

	if (TextureImage[0]) {
		if (TextureImage[0]->data) {
			free(TextureImage[0]->data);
		}

		free(TextureImage[0]);
	}

	return Status;
}

int InitGL(GLvoid) {
	LoadGLTextures();

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	return TRUE;
}

void DrawGLScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

    glTranslatef( 0.0f, 0.0f, g_fDistance );
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

//	glTranslatef(0, 0, -4);
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glutSolidTeapot(1.0);

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

    if( bMousing )
    {
		g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
        g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
    }

    ptLastMousePosit.x = ptCurrentMousePosit.x;
    ptLastMousePosit.y = ptCurrentMousePosit.y;

    glutPostRedisplay();
}

void MyReshape (int w, int h) {
     glViewport(0,0,w,h);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();

     gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 0.1, 100.0);
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity();
}

void main (int argc, char ** argv) {
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
     glutInitWindowPosition(100, 100);
     glutInitWindowSize(800, 600);
     glutCreateWindow("Auto Texture Mapping");
     InitGL();
     glutDisplayFunc(DrawGLScene);
     glutReshapeFunc(MyReshape);
	 glutMouseFunc(MyMouse);
	 glutMotionFunc(MyMotion);

     glutMainLoop( );
}