#include <windows.h>
#include <stdio.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <glaux.h>

GLfloat     xrot = 30, yrot = 30, xspeed = 1, yspeed = 1, z = -10.0f;
GLuint     filter, texture[6], object = 1;
GLUquadricObj *quadratic;
static bool bMousingL, bMousingR;

AUX_RGBImageRec *LoadBMP(char *Filename) {
     FILE *File = NULL;

     if (!Filename)     return NULL;
     File = fopen(Filename,"r");

     if (File) {
          fclose(File);
          return auxDIBImageLoad(Filename);
     }

     return NULL;
}

void LoadGLTextures() {
     AUX_RGBImageRec *TextureImage[2];
     memset(TextureImage,0,sizeof(void *)*2);

     if ((TextureImage[0] = LoadBMP("Data/Background.bmp")) && 
          (TextureImage[1] = LoadBMP("Data/SphereMap.bmp"))) {
          glGenTextures(6, &texture[0]);

          for (int i=0; i<2; i++) {
               // Create Nearest Filtered Texture
               glBindTexture(GL_TEXTURE_2D, texture[i]);
               glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
               glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
               glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[i]->sizeX,
                              TextureImage[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
                              TextureImage[i]->data);

               // Create Linear Filtered Texture
               glBindTexture(GL_TEXTURE_2D, texture[i+2]);
               glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
               glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
               glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[i]->sizeX,
                              TextureImage[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
                              TextureImage[i]->data);

               // Create MipMapped Texture
               glBindTexture(GL_TEXTURE_2D, texture[i+4]);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
               glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                               GL_LINEAR_MIPMAP_NEAREST);
               gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[i]->sizeX,
                              TextureImage[i]->sizeY, GL_RGB, GL_UNSIGNED_BYTE,
                              TextureImage[i]->data);
          }

          for (int i=0; i<2; i++) {
               if (TextureImage[i]) {
                    if (TextureImage[i]->data) {
                         free(TextureImage[i]->data);
                    }
                    free(TextureImage[i]);
               }
          }
     }
}

void glDrawCube() {
     glBegin(GL_QUADS);

     // Front Face
     glNormal3f( 0.0f, 0.0f, 0.5f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.5f, -1.5f,  1.5f);
     glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.5f, -1.5f,  1.5f);
     glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.5f,  1.5f,  1.5f);
     glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.5f,  1.5f,  1.5f);

     // Back Face
     glNormal3f( 0.0f, 0.0f,-0.5f);
     glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.5f, -1.5f, -1.5f);
     glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.5f,  1.5f, -1.5f);
     glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.5f,  1.5f, -1.5f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.5f, -1.5f, -1.5f);

     // Top Face
     glNormal3f( 0.0f, 0.5f, 0.0f);
     glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.5f,  1.5f, -1.5f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.5f,  1.5f,  1.5f);
     glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.5f,  1.5f,  1.5f);
     glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.5f,  1.5f, -1.5f);

     // Bottom Face
     glNormal3f( 0.0f,-0.5f, 0.0f);
     glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.5f, -1.5f, -1.5f);
     glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.5f, -1.5f, -1.5f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.5f, -1.5f,  1.5f);
     glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.5f, -1.5f,  1.5f);

     // Right Face
     glNormal3f( 0.5f, 0.0f, 0.0f);
     glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.5f, -1.5f, -1.5f);
     glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.5f,  1.5f, -1.5f);
     glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.5f,  1.5f,  1.5f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.5f, -1.5f,  1.5f);

     // Left Face
     glNormal3f(-0.5f, 0.0f, 0.0f);
     glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.5f, -1.5f, -1.5f);
     glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.5f, -1.5f,  1.5f);
     glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.5f,  1.5f,  1.5f);
     glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.5f,  1.5f, -1.5f);

     glEnd();
}

void DrawGLScene() {
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glLoadIdentity();

     glTranslatef(0.0f, 0.0f, z);

     glEnable(GL_TEXTURE_GEN_S);
     glEnable(GL_TEXTURE_GEN_T);

     glBindTexture(GL_TEXTURE_2D, texture[filter+(filter+1)]);
     glPushMatrix();
          glRotatef(xrot,1.0f,0.0f,0.0f);
          glRotatef(yrot,0.0f,1.0f,0.0f);

          switch(object) {
               case 0:
                    glDrawCube();
                    break;
               case 1:
                   glTranslatef(0.0f,0.0f,-1.5f);
                    gluCylinder(quadratic,2.0f,2.0f,4.0f,32,32);
                    break;
               case 2:
                    gluSphere(quadratic,2.5,32,32);
                    break;
               case 3:
                    glutSolidTeapot(1.5);
                    break;
          };
     glPopMatrix();

     glDisable(GL_TEXTURE_GEN_S);
     glDisable(GL_TEXTURE_GEN_T);

     glBindTexture(GL_TEXTURE_2D, texture[filter*2]);
     glPushMatrix();
          glTranslatef(0.0f, 0.0f, -24.0f);
          glBegin(GL_QUADS);
               glNormal3f( 0.0f, 0.0f, 1.0f);
               glTexCoord2f(0.0f, 0.0f); glVertex3f(-13.3f, -10.0f,  10.0f);
               glTexCoord2f(1.0f, 0.0f); glVertex3f( 13.3f, -10.0f,  10.0f);
               glTexCoord2f(1.0f, 1.0f); glVertex3f( 13.3f,  10.0f,  10.0f);
               glTexCoord2f(0.0f, 1.0f); glVertex3f(-13.3f,  10.0f,  10.0f);
          glEnd();
     glPopMatrix();

     glutSwapBuffers();
}

void MyReshape(int w, int h) {
     glViewport(0,0,w,h);
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();

     gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 100.0);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
}

void MySpecial(int key, int x, int y) {
     if(key == GLUT_KEY_UP) {
          xspeed-=0.01f;
     } else if(key == GLUT_KEY_DOWN) {
          xspeed+=0.01f;
     } else if(key == GLUT_KEY_LEFT) {
          yspeed-=0.01f;
     } else if(key == GLUT_KEY_RIGHT) {
          yspeed+=0.01f;
     } else if(key == GLUT_KEY_PAGE_UP) {
          z-=1.02f; 
     } else if(key == GLUT_KEY_PAGE_DOWN) {
          z+=1.02f;
     }

     glutPostRedisplay();
}

void MyMouse(int button, int state, int x, int y) {
     switch (button) {
          case GLUT_LEFT_BUTTON:
               if (state == GLUT_DOWN) {
                    bMousingL = true;
                    object++;
                    if(object>3) object=0; 
                } else 
                     bMousingL = false;
                break;
           case GLUT_MIDDLE_BUTTON:
           case GLUT_RIGHT_BUTTON:
                if (state == GLUT_DOWN) {
                     bMousingR = true;
                } else 
                     bMousingR = false;
               break;
           default:
                break;
     }

     glutPostRedisplay();
}

void MyMotion(int x, int y) {
     if( bMousingR ) {
          xrot+=xspeed;
          yrot+=yspeed;
    }

    glutPostRedisplay();
}

void InitGL() {
     GLfloat Light0_Ambient[]={ 0.5f, 0.5f, 0.5f, 1.0f };
     GLfloat Light0_Diffuse[]={ 1.0f, 1.0f, 1.0f, 1.0f };
     GLfloat Light0_Position[]={ 0.0f, 0.0f, 2.0f, 1.0f };

     glLightfv(GL_LIGHT0, GL_AMBIENT, Light0_Ambient);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, Light0_Diffuse);
     glLightfv(GL_LIGHT0, GL_POSITION,Light0_Position);

     LoadGLTextures();

     glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glClearDepth(1.0f);
     glDepthFunc(GL_LEQUAL);
     glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
     glEnable(GL_DEPTH_TEST);
     glEnable(GL_TEXTURE_2D);
     glShadeModel(GL_SMOOTH);

     quadratic=gluNewQuadric();
     gluQuadricNormals(quadratic, GLU_SMOOTH);
     gluQuadricTexture(quadratic, GL_TRUE);

     glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
     glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
}

void main(int argc, char **argv) {
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
     glutInitWindowPosition(100, 100);
     glutInitWindowSize(800, 600);
     glutCreateWindow("Environment Mapping");
     InitGL();
     glutDisplayFunc(DrawGLScene);
     glutReshapeFunc(MyReshape);
     glutSpecialFunc(MySpecial);
     glutMouseFunc(MyMouse);
     glutMotionFunc(MyMotion);

     glutMainLoop( );
}