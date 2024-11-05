#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "glaux.h"
#include <gl/glut.h>
#include <gl/glu.h>
#include <glext.h>

#define MAX_EMBOSS (GLfloat)0.008f

bool	   emboss = false;
bool       bumps = true;

GLuint     filter   = 1;
GLuint     texture[3];
GLuint     bump[3];
GLuint     invbump[3];

GLfloat Gray[] = {0.5f,0.5f,0.5f,1.0f};

float g_fDistance = -5.0f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

GLfloat LightAmbient[] = { 0.2f, 0.2f, 0.2f};
GLfloat LightDiffuse[]  = { 1.0f, 1.0f, 1.0f};
GLfloat LightPosition[] = { 0.0f, 0.0f, 2.0f};

// Data Contains The Faces For The Cube In Format 2xTexCoord, 3xVertex;
// Note That The Tesselation Of The Cube Is Only Absolute Minimum.
GLfloat data[]= {
     // FRONT FACE
     0.0f, 0.0f,-1.0f, -1.0f, +1.0f,
     1.0f, 0.0f,+1.0f, -1.0f, +1.0f,
     1.0f, 1.0f,+1.0f, +1.0f, +1.0f,
     0.0f, 1.0f,-1.0f, +1.0f, +1.0f,

     // BACK FACE
     1.0f, 0.0f,-1.0f, -1.0f, -1.0f,
     1.0f, 1.0f,-1.0f, +1.0f, -1.0f,
     0.0f, 1.0f,+1.0f, +1.0f, -1.0f,
     0.0f, 0.0f,+1.0f, -1.0f, -1.0f,

     // Top Face
     0.0f, 1.0f,-1.0f, +1.0f, -1.0f,
     0.0f, 0.0f,-1.0f, +1.0f, +1.0f,
     1.0f, 0.0f,+1.0f, +1.0f, +1.0f,
     1.0f, 1.0f,+1.0f, +1.0f, -1.0f,

     // Bottom Face
     1.0f, 1.0f,-1.0f, -1.0f, -1.0f,
     0.0f, 1.0f,+1.0f, -1.0f, -1.0f,
     0.0f, 0.0f,+1.0f, -1.0f, +1.0f,
     1.0f, 0.0f,-1.0f, -1.0f, +1.0f,

     // Right Face
     1.0f, 0.0f,+1.0f, -1.0f, -1.0f,
     1.0f, 1.0f,+1.0f, +1.0f, -1.0f,
     0.0f, 1.0f,+1.0f, +1.0f, +1.0f,
     0.0f, 0.0f,+1.0f, -1.0f, +1.0f,

     // Left Face
     0.0f, 0.0f,-1.0f, -1.0f, -1.0f,
     1.0f, 0.0f,-1.0f, -1.0f,  1.0f,
     1.0f, 1.0f,-1.0f,  1.0f,  1.0f,
     0.0f, 1.0f,-1.0f,  1.0f, -1.0f
};

int LoadGLTextures() {
     bool status = true;
     AUX_RGBImageRec *Image = NULL;
     char *alpha = NULL;

     if (Image=auxDIBImageLoad("Data/Base.bmp")) {
          glGenTextures(3, texture);// Create Three Textures
          glBindTexture(GL_TEXTURE_2D, texture[0]);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Image->sizeX, Image->sizeY, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, Image->data);
          // Use GL_RGB8 Instead Of "3" In glTexImage2D. Also Defined By GL: GL_RGBA8 Etc.
          // NEW: Now Creating GL_RGBA8 Textures, Alpha Is 1.0f
          glBindTexture(GL_TEXTURE_2D, texture[1]);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Image->sizeX, Image->sizeY, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, Image->data);

          // Create MipMapped Texture
          glBindTexture(GL_TEXTURE_2D, texture[2]);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
                         GL_LINEAR_MIPMAP_NEAREST);
          gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, Image->sizeX, Image->sizeY,
                         GL_RGB, GL_UNSIGNED_BYTE, Image->data);
     } else
          status = false;

     if (Image) {
          if (Image->data) delete Image->data;// If Texture Image Exists
          delete Image;
          Image=NULL;
     }

     if (Image=auxDIBImageLoad("Data/Bump.bmp")) {
          glPixelTransferf(GL_RED_SCALE,0.5f);
          glPixelTransferf(GL_GREEN_SCALE,0.5f);
          glPixelTransferf(GL_BLUE_SCALE,0.5f);

          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
          glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,Gray);

          glGenTextures(3, bump);// Create Three Textures

          // Create Nearest Filtered Texture
          glBindTexture(GL_TEXTURE_2D, bump[0]);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Image->sizeX, Image->sizeY, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, Image->data);

          // Create Linear Filtered Texture
          glBindTexture(GL_TEXTURE_2D, bump[1]);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Image->sizeX, Image->sizeY, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, Image->data);

          // Create MipMapped Texture
          glBindTexture(GL_TEXTURE_2D, bump[2]);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                         GL_LINEAR_MIPMAP_NEAREST);
          gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, Image->sizeX, Image->sizeY,
                         GL_RGB, GL_UNSIGNED_BYTE, Image->data);

          for (int i=0; i<3*Image->sizeX*Image->sizeY; i++)// Invert The Bumpmap
               Image->data[i]=255-Image->data[i];

          glGenTextures(3, invbump);// Create Three Textures

          // Create Nearest Filtered Texture
          glBindTexture(GL_TEXTURE_2D, invbump[0]);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Image->sizeX, Image->sizeY, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, Image->data);

          // Create Linear Filtered Texture
          glBindTexture(GL_TEXTURE_2D, invbump[1]);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Image->sizeX, Image->sizeY, 0,
                         GL_RGB, GL_UNSIGNED_BYTE, Image->data);

          // Create MipMapped Texture
          glBindTexture(GL_TEXTURE_2D, invbump[2]);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                         GL_LINEAR_MIPMAP_NEAREST);
          gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, Image->sizeX, Image->sizeY,
                         GL_RGB, GL_UNSIGNED_BYTE, Image->data);

          glPixelTransferf(GL_RED_SCALE,1.0f);
          glPixelTransferf(GL_GREEN_SCALE,1.0f);
          glPixelTransferf(GL_BLUE_SCALE,1.0f);
     } else
          status = false;

     if (Image) {
          if (Image->data) delete Image->data;
          delete Image;
     }

     return status;
}

void doCube (void) {
     glBegin(GL_QUADS);
          // Front Face
          glNormal3f( 0.0f, 0.0f, +1.0f);
          for (int i=0; i<4; i++) {
               glTexCoord2f(data[5*i],data[5*i+1]);
               glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
          }

          // Back Face
          glNormal3f( 0.0f, 0.0f,-1.0f);
          for (int i=4; i<8; i++) {
               glTexCoord2f(data[5*i],data[5*i+1]);
               glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
          }

          // Top Face
          glNormal3f( 0.0f, 1.0f, 0.0f);
          for (int i=8; i<12; i++) {
               glTexCoord2f(data[5*i],data[5*i+1]);
               glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
          }

          // Bottom Face
          glNormal3f( 0.0f,-1.0f, 0.0f);
          for (int i=12; i<16; i++) {
               glTexCoord2f(data[5*i],data[5*i+1]);
               glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
          }

          // Right face
          glNormal3f( 1.0f, 0.0f, 0.0f);
          for (int i=16; i<20; i++) {
               glTexCoord2f(data[5*i],data[5*i+1]);
               glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
          }

          // Left Face
          glNormal3f(-1.0f, 0.0f, 0.0f);
          for (int i=20; i<24; i++) {
               glTexCoord2f(data[5*i],data[5*i+1]);
               glVertex3f(data[5*i+2],data[5*i+3],data[5*i+4]);
          }
     glEnd();
}

// Calculates v=vM, M Is 4x4 In Column-Major, v Is 4dim. Row (i.e. "Transposed")
void VMatMult(GLfloat *M, GLfloat *v) {
     GLfloat res[3];
     res[0]=M[ 0]*v[0]+M[ 1]*v[1]+M[ 2]*v[2]+M[ 3]*v[3];
     res[1]=M[ 4]*v[0]+M[ 5]*v[1]+M[ 6]*v[2]+M[ 7]*v[3];
     res[2]=M[ 8]*v[0]+M[ 9]*v[1]+M[10]*v[2]+M[11]*v[3];;
     v[0]=res[0];
     v[1]=res[1];
     v[2]=res[2];
     v[3]=M[15];
}

void SetUpBumps(GLfloat *n, GLfloat *c, GLfloat *l, GLfloat *s, GLfloat *t) {
     GLfloat v[3];// Vertex From Current Position To Light
     GLfloat lenQ;// Used To Normalize

     // Calculate v From Current Vector c To Lightposition And Normalize v
     v[0]=l[0]-c[0];
     v[1]=l[1]-c[1];
     v[2]=l[2]-c[2];
     lenQ=(GLfloat) sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
     v[0]/=lenQ;v[1]/=lenQ;v[2]/=lenQ;
     // Project v Such That We Get Two Values Along Each Texture-Coordinat Axis.
     c[0]=(s[0]*v[0]+s[1]*v[1]+s[2]*v[2])*MAX_EMBOSS;
     c[1]=(t[0]*v[0]+t[1]*v[1]+t[2]*v[2])*MAX_EMBOSS;
}

void DrawGLScene() {
     GLfloat c[4]={0.0f,0.0f,0.0f,1.0f};// Holds Current Vertex
     GLfloat n[4]={0.0f,0.0f,0.0f,1.0f};// Normalized Normal Of Current Surface
     GLfloat s[4]={0.0f,0.0f,0.0f,1.0f};// s-Texture Coordinate Direction, Normalized
     GLfloat t[4]={0.0f,0.0f,0.0f,1.0f};// t-Texture Coordinate Direction, Normalized
     GLfloat l[4];// Holds Our Lightposition To Be Transformed Into Object Space
     GLfloat Minv[16];// Holds The Inverted Modelview Matrix To Do So.

     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glMatrixMode( GL_MODELVIEW );
     glLoadIdentity();

     glTranslatef( 0.0f, 0.0f, g_fDistance );
     glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
     glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

     // Transform The Lightposition Into Object Coordinates:
     l[0]=LightPosition[0];
     l[1]=LightPosition[1];
     l[2]=LightPosition[2];
     l[3]=1.0f;// Homogenous Coordinate
     VMatMult(Minv,l);

     /*PASS#1: Use Texture "Bump"
     No Blend
     No Lighting
     No Offset Texture-Coordinates */
     glBindTexture(GL_TEXTURE_2D, bump[filter]);
     glDisable(GL_BLEND);
     glDisable(GL_LIGHTING);
     doCube();

     /* PASS#2:Use Texture "Invbump"
     Blend GL_ONE To GL_ONE
     No Lighting
     Offset Texture Coordinates 
     */
     glBindTexture(GL_TEXTURE_2D,invbump[filter]);
     glBlendFunc(GL_ONE,GL_ONE);
     glDepthFunc(GL_LEQUAL);
     glEnable(GL_BLEND);

     glBegin(GL_QUADS);
          // Front Face
          n[0]=0.0f;n[1]=0.0f;n[2]=1.0f;
          s[0]=1.0f;s[1]=0.0f;s[2]=0.0f;
          t[0]=0.0f;t[1]=1.0f;t[2]=0.0f;
          for (int i=0; i<4; i++) {
               c[0]=data[5*i+2];
               c[1]=data[5*i+3];
               c[2]=data[5*i+4];
               SetUpBumps(n,c,l,s,t);
               glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]); 
               glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);
          }

          // Back Face
          n[0]=0.0f;n[1]=0.0f;n[2]=-1.0f;
          s[0]=-1.0f;s[1]=0.0f;s[2]=0.0f;
          t[0]=0.0f;t[1]=1.0f;t[2]=0.0f;
          for (int i=4; i<8; i++) {
               c[0]=data[5*i+2];
               c[1]=data[5*i+3];
               c[2]=data[5*i+4];
               SetUpBumps(n,c,l,s,t);
               glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]); 
               glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);
          }

          // Top Face
          n[0]=0.0f;n[1]=1.0f;n[2]=0.0f;
          s[0]=1.0f;s[1]=0.0f;s[2]=0.0f;
          t[0]=0.0f;t[1]=0.0f;t[2]=-1.0f;
          for (int i=8; i<12; i++) {
               c[0]=data[5*i+2];
               c[1]=data[5*i+3];
               c[2]=data[5*i+4];
               SetUpBumps(n,c,l,s,t);
               glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]); 
               glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);
          }

          // Bottom Face
          n[0]=0.0f;n[1]=-1.0f;n[2]=0.0f;
          s[0]=-1.0f;s[1]=0.0f;s[2]=0.0f;
          t[0]=0.0f;t[1]=0.0f;t[2]=-1.0f;
          for (int i=12; i<16; i++) {
               c[0]=data[5*i+2];
               c[1]=data[5*i+3];
               c[2]=data[5*i+4];
               SetUpBumps(n,c,l,s,t);
               glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]); 
               glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);
          }

          // Right Face
          n[0]=1.0f;n[1]=0.0f;n[2]=0.0f;
          s[0]=0.0f;s[1]=0.0f;s[2]=-1.0f;
          t[0]=0.0f;t[1]=1.0f;t[2]=0.0f;
          for (int i=16; i<20; i++) {
               c[0]=data[5*i+2];
               c[1]=data[5*i+3];
               c[2]=data[5*i+4];
               SetUpBumps(n,c,l,s,t);
               glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]); 
               glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);
          }

          // Left Face
          n[0]=-1.0f;n[1]=0.0f;n[2]=0.0f;
          s[0]=0.0f;s[1]=0.0f;s[2]=1.0f;
          t[0]=0.0f;t[1]=1.0f;t[2]=0.0f;
          for (int i=20; i<24; i++) {
               c[0]=data[5*i+2];
               c[1]=data[5*i+3];
               c[2]=data[5*i+4];
               SetUpBumps(n,c,l,s,t);
               glTexCoord2f(data[5*i]+c[0], data[5*i+1]+c[1]); 
               glVertex3f(data[5*i+2], data[5*i+3], data[5*i+4]);
          }
     glEnd();

     /* PASS#3:Use Texture "Base"
     Blend GL_DST_COLOR To GL_SRC_COLOR (Multiplies By 2)
     Lighting Enabled
     No Offset Texture-Coordinates
     */
     if (!emboss) {
          glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
          glBindTexture(GL_TEXTURE_2D,texture[filter]);
          glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
          glEnable(GL_LIGHTING);
          doCube();
     }

     glutSwapBuffers();
}

int InitGL() {
     LoadGLTextures();

     glEnable(GL_TEXTURE_2D);
     glShadeModel(GL_SMOOTH);
     glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
     glClearDepth(1.0f);
     glEnable(GL_DEPTH_TEST);
     glDepthFunc(GL_LEQUAL);
     glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

     glLightfv( GL_LIGHT1, GL_AMBIENT, LightAmbient);
     glLightfv( GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
     glLightfv( GL_LIGHT1, GL_POSITION, LightPosition);

     glEnable(GL_LIGHT1);

     return true;
}

void MyReshape (int w, int h) {
     glViewport (0.0, 0.0, (GLfloat) w, (GLfloat) h);
     glMatrixMode (GL_PROJECTION);
     glLoadIdentity ();

     gluPerspective (45.0, (GLfloat) w / (GLfloat) h, 0.01, 100.0);
     glMatrixMode (GL_MODELVIEW);
     glLoadIdentity();
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

void MySpecial(int key, int x, int y) {
     if(key == GLUT_KEY_UP) {
          g_fDistance -= 0.1f;
     } else if(key == GLUT_KEY_DOWN) {
          g_fDistance += 0.1f;
     }

     glutPostRedisplay();
}

int main (int argc, char **argv) {
     glutInit (&argc, argv);
     glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB | GLUT_STENCIL);
     glutInitWindowPosition(100, 100);
     glutInitWindowSize(800, 600);
     glutCreateWindow ("Bump Mapping");
     InitGL();
     glutDisplayFunc (DrawGLScene);
     glutReshapeFunc (MyReshape);
     glutMouseFunc(MyMouse);
     glutMotionFunc(MyMotion);
     glutSpecialFunc(MySpecial);

     glutMainLoop ();

     return 0;
}