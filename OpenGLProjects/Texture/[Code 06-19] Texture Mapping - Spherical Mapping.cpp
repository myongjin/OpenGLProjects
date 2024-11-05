#include <stdio.h>
#include "glaux.h"
#include <gl/glut.h>
#include <gl/glu.h>

#define TEXTURE_NUM 1
GLuint texture[TEXTURE_NUM];
char texture_name[TEXTURE_NUM][20]={"Data/homework.bmp"};

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

float g_fDistance = -4.0f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;

void MyReshape(int w, int h){	
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(40.0, (GLfloat)w/(GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

    glTranslatef( 0.0f, 0.0f, g_fDistance );
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

    glBindTexture(GL_TEXTURE_2D, texture[0]);

     //GLUquadricObj *obj = gluNewQuadric();
     //gluQuadricTexture(obj, GL_TRUE); 
     //gluQuadricNormals(obj, GLU_SMOOTH);
     //
     //glPushMatrix();
     //   //cylinder
     //    gluCylinder(obj, 0.7, 0.7, 1.5, 50, 50);
     //    gluDisk(obj, 0.0, 0.7, 50, 1); 
	    // glTranslatef(0.0, 0.0, 1.5);
     //    gluDisk(obj, 0.0, 0.7, 50, 1); 
	    // glTranslatef(0.0, 0.0, -1.5); 
     //glPopMatrix();
     //gluDeleteQuadric(obj);

     //
//gluSphere(obj, 1, 50, 50);
glutSolidTeapot(1);
    //glutSolidCube(1.5);


	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

    glutSwapBuffers();
}

AUX_RGBImageRec * LoadBMP(char *Filename) {
    FILE * File = NULL;
    if(!Filename) return NULL;
    File = fopen(Filename,"r");
    if(File) {
        fclose(File);
        return auxDIBImageLoad(Filename);
    }
    return NULL;
}

int LoadGLTextures() {
	int Status = TRUE, i;
	AUX_RGBImageRec *TextureImage;

	for(i=0;i<TEXTURE_NUM;i++) {
     TextureImage=NULL;
     if (TextureImage=LoadBMP(texture_name[i])) {
          glGenTextures(1, &texture[i]);
          glBindTexture(GL_TEXTURE_2D, texture[i]);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
          glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage->sizeX, TextureImage->sizeY,
                         0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage->data);
     } else {
          MessageBox(NULL,"Image file has a error !",texture_name[i],MB_ICONERROR);
          Status=false;
     }

     if (TextureImage) {
          if (TextureImage->data) free(TextureImage->data);
          free(TextureImage);
     }
   }
   return Status;
}

void Init() {
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);

    LoadGLTextures();
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

void main(int argc, char ** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Spherical Mapping");
    glutDisplayFunc(MyDisplay);
    glutReshapeFunc(MyReshape);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);
	Init();
    glutMainLoop( );
}