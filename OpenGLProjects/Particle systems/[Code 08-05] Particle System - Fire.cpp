#include <windows.h>          
#include <stdio.h>    
#include <math.h>            
#include <glaux.h>
#include <gl/glut.h>                     
#include <gl/glu.h>
  
#include <vector>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;
//Release 모드시 뜨는 오류 잡기 위함
#pragma comment(lib, "advapi32.lib")

#define MAX_PARTICLES   1000                   
float slowdown = 2.0f, xspeed, yspeed, zoom = -20.0f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

bool upflag, downflag, rightflag, leftflag, reset;


typedef struct {
    bool    active;
    float   life;
    float   fade;
    vec3 color;
    vec3 pos;
    vec3 vel;
    vec3 force;
}
particles;

particles particle[MAX_PARTICLES];    
 
GLuint      texture[1];       
bool		keys[256];            

AUX_RGBImageRec *LoadBMP(CHAR *Filename) {   
    FILE *File=NULL;                                      
    if (!Filename) {
        return NULL;                                 
    }   
    File=fopen(Filename,"r");                           
    if (File) {   
        fclose(File);                                  
        return auxDIBImageLoadA(Filename);            
    }   
    return NULL;                                       
}   
   
void LoadGLTextures() {   
    AUX_RGBImageRec *TextureImage[1];                  
    memset(TextureImage,0,sizeof(void *)*1);               
	
	if (TextureImage[0]=LoadBMP("Data/Particle.bmp")) {  
        glGenTextures(1, &texture[0]);
        glBindTexture(GL_TEXTURE_2D, texture[0]);       
        glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[0]->data);   
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);     
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);     
    }   
   
    if (TextureImage[0]) {   
        if (TextureImage[0]->data) {   
            free(TextureImage[0]->data);            
        }   
   
        free(TextureImage[0]);                           
    }   
}   
   

void SetParticle(int loop)
{
    particle[loop].active = true;
    particle[loop].life = 1.0f;
    particle[loop].fade = float(rand() % 80) / 1000.0f + 0.003f;
    particle[loop].color = vec3(1.0f, 0.3f, 0.2f);
    particle[loop].vel = vec3((rand() % 50) - 25.0f, (rand() % 50) - 25.0f, (rand() % 50) - 25.0f);
    particle[loop].force = vec3(0, 0.6, 0);

    if (loop % 2 == 0) {
        particle[loop].pos = vec3(1.0f, 0.0f, 0.0f);
    }
    if (loop % 2 == 1) {
        particle[loop].pos = vec3(0.0f, 0.0f, 0.0f);
    }
}
int InitGL(GLvoid) {   
    upflag = false;
    downflag = false;
    leftflag = false;
    rightflag = false;
    reset = false;
    
    LoadGLTextures();

    glEnable(GL_BLEND);   
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   
    glEnable(GL_ALPHA_TEST);   
    glAlphaFunc(GL_GREATER, 0);   
    glEnable(GL_TEXTURE_2D);                           
    glShadeModel(GL_SMOOTH);                       
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);                 
    glClearDepth(1.0f);
    glDisable(GL_DEPTH_TEST);     

    for (int loop=0;loop<MAX_PARTICLES;loop++) {   
        SetParticle(loop);
    }   

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);     
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);   
    glEnable(GL_BLEND);   
    return TRUE;                                       
}   
   
void DrawGLScene(GLvoid) {  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
    glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();									

    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

    for (int loop=0;loop<MAX_PARTICLES;loop++) {   
        if (particle[loop].active) {   
            float x = particle[loop].pos[0];
            float y = particle[loop].pos[1];
            float z = particle[loop].pos[2] +zoom;

            //텍스쳐 이용해 particle 그리기
            //glDisable(GL_BLEND);
            glBindTexture(GL_TEXTURE_2D, texture[0]);   
            glColor4f(particle[loop].color[0], particle[loop].color[1], particle[loop].color[2], particle[loop].life);

            glBegin(GL_QUADS);
                glTexCoord2d(0,0); glVertex3f(x-0.7f,y-2.7f,z);    
                glTexCoord2d(1,0); glVertex3f(x+0.7f,y-2.7f,z);
                glTexCoord2d(1,1); glVertex3f(x+0.7f,y+0.7f,z);    
                glTexCoord2d(0,1); glVertex3f(x-0.7f,y+0.7f,z); 
            glEnd();  
             

            //속도와 가속도를 이용해 particle 위치 업데이트
            //life값 업데이트
            particle[loop].pos    += particle[loop].vel/(slowdown*1000);       
            particle[loop].vel   += particle[loop].force;                                    
            particle[loop].life -= particle[loop].fade; 

            //불꽃 생명이 다한 경우, 다시 불 시작 위치로 이동 및 생명 초기화
            if (particle[loop].life < 0.0f) {   
                SetParticle(loop);
            }   
            
            //키보드 조작에 따른 힘 추가
            if (upflag && (particle[loop].force[1] < 1.5f)) particle[loop].force[1] += 0.01f;
            if (downflag && (particle[loop].force[1] > -1.5f)) particle[loop].force[1] -= 0.01f;
            if (rightflag && (particle[loop].force[0] < 1.5f)) particle[loop].force[0] += 0.01f;
            if (leftflag && (particle[loop].force[0] > -1.5f)) particle[loop].force[0] -= 0.01f;
        }   
    }   

	glutPostRedisplay();
	glutSwapBuffers();
}   
  
void MySpecial(int key, int x, int y) {
	if(key == GLUT_KEY_UP) {
		if(yspeed < 200) yspeed += 1.0f;
	} else if(key == GLUT_KEY_DOWN) {
		if(yspeed > -200) yspeed -= 1.0f;
	} else if(key == GLUT_KEY_LEFT) {
		if(xspeed > -200) xspeed -= 1.0f;
	} else if(key == GLUT_KEY_RIGHT) {
		if(xspeed < 200) xspeed += 1.0f; 
	} else if(key == GLUT_KEY_PAGE_UP) {
		zoom += 0.1f;
	} else if(key == GLUT_KEY_PAGE_DOWN) {
		zoom -= 0.1f;
	} else if(key == GLUT_KEY_HOME) {
		if(slowdown > 1.0f) slowdown -= 0.01f; 
	} else if(key == GLUT_KEY_END) {
		if(slowdown < 4.0f) slowdown += 0.01f;
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
    gluPerspective(40.0, (GLfloat)w/(GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void MyKeyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        upflag = !upflag;
        break;
    case 's':
        downflag = !downflag;
        break;
    case 'a':
        leftflag = !leftflag;
        break;
    case 'd':
        rightflag = !rightflag;
        break;

    case 'r':
        reset = !reset;
        break;

    default:
        break;
    }
    glutPostRedisplay();
}

void main(int argc, char **argv){
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Particle - Fire");
	InitGL();
    glutDisplayFunc(DrawGLScene);
    glutReshapeFunc(MyReshape);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);
	glutSpecialFunc(MySpecial);
    glutKeyboardFunc(MyKeyboard);
    glutMainLoop( );
}