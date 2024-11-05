#include <Windows.h>
#include <stdlib.h>
#include <math.h>
#include <gl/glut.h>

#pragma comment(lib, "advapi32.lib")

#define PI 3.1415265359
#define RandomFactor 2.0

GLfloat WaterHeight = -0.45;//0.45;

bool DoUpdateScene = true;

float g_fDistance = -4.5f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

struct SVertex {
	GLfloat x,y,z;
};

class CDrop {
	private:
		GLfloat time;
		SVertex ConstantSpeed;
		GLfloat AccFactor;
	public:
		void SetConstantSpeed (SVertex NewSpeed);
		void SetAccFactor(GLfloat NewAccFactor);
		void SetTime(GLfloat NewTime);
		void GetNewPosition(SVertex * PositionVertex);
};

void CDrop::SetConstantSpeed(SVertex NewSpeed) {
	ConstantSpeed = NewSpeed;
}

void CDrop::SetAccFactor (GLfloat NewAccFactor) {
	AccFactor = NewAccFactor;
}

void CDrop::SetTime(GLfloat NewTime) {
	time = NewTime;
}

void CDrop::GetNewPosition(SVertex * PositionVertex) {
	SVertex Position;
	time += 1.0;
	Position.x = ConstantSpeed.x * time;
	Position.y = ConstantSpeed.y * time - AccFactor * time * time;
	Position.z = ConstantSpeed.z * time;
	PositionVertex->x = Position.x;
	PositionVertex->y = Position.y + WaterHeight;
	PositionVertex->z = Position.z;

	if (Position.y < 0.0) {
		time = time - int(time);
		if (time > 0.0) time -= 1;//10.0;
	}		
}

CDrop * FountainDrops;
SVertex * FountainVertices;
GLint Steps = 3;
GLint RaysPerStep = 8;				// 물줄기가 뻗어나가는 개수
GLint DropsPerRay = 50;				// 떨어지는 물방울 입자의 개수
GLfloat DropsComplete = Steps * RaysPerStep * DropsPerRay;
GLfloat AngleOfDeepestStep = 80;	// 물줄기가 뻗어나가는 각도
GLfloat AccFactor = 0.011;

GLfloat GetRandomFloat(GLfloat range) {
	return (GLfloat)rand() / (GLfloat)RAND_MAX * range * RandomFactor;
}

void InitFountain() {
	FountainDrops = new CDrop [ DropsComplete ];
	FountainVertices = new SVertex [ DropsComplete ];
	SVertex NewSpeed;
	GLfloat DropAccFactor;
	GLfloat TimeNeeded;
	GLfloat StepAngle;
	GLfloat RayAngle;

	for (int k = 0; k <Steps; k++) {
		for (int j = 0; j < RaysPerStep; j++) {
			for (int i = 0; i < DropsPerRay; i++) {
				DropAccFactor = AccFactor + GetRandomFloat(0.0005);
				StepAngle = AngleOfDeepestStep + (90.0-AngleOfDeepestStep) 
						* GLfloat(k) / (Steps-1) + GetRandomFloat(0.2+0.8*(Steps-k-1)/(Steps-1));

				//This is the speed caused by the step:
				NewSpeed.x = cos ( StepAngle * PI / 180.0) * (0.2+0.04*k);
				NewSpeed.y = sin ( StepAngle * PI / 180.0) * (0.2+0.04*k);

				//This is the speed caused by the ray:	
				RayAngle = (GLfloat)j / (GLfloat)RaysPerStep * 360.0;

				//for the next computations "NewSpeed.x" is the radius. Care! Dont swap the two
				//lines, because the second one changes NewSpeed.x!
				NewSpeed.z = NewSpeed.x * sin ( RayAngle * PI /180.0);
				NewSpeed.x = NewSpeed.x * cos ( RayAngle * PI /180.0);
				
				//Calculate how many steps are required, that a drop comes out and falls down again
				TimeNeeded = NewSpeed.y/ DropAccFactor;
				FountainDrops[i+j*DropsPerRay+k*DropsPerRay*RaysPerStep].SetConstantSpeed ( NewSpeed );
				FountainDrops[i+j*DropsPerRay+k*DropsPerRay*RaysPerStep].SetAccFactor (DropAccFactor);
				FountainDrops[i+j*DropsPerRay+k*DropsPerRay*RaysPerStep].SetTime(TimeNeeded * i / DropsPerRay);
			}
		}
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, FountainVertices);						
}

void DrawFountain() {
	glColor4f(0.8,0.8,0.8,0.8);

	if (DoUpdateScene) {
		for (int i = 0; i < DropsComplete; i++) {
			FountainDrops[i].GetNewPosition(&FountainVertices[i]);
		}
	}

	glDrawArrays( GL_POINTS, 0, DropsComplete );
}

void Display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0,0.0,-5.0);	
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1,0.1,0.1,0.0);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	DrawFountain();

	glTranslatef(0.0, -0.45, 0.0);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glColor3f(0.3, 0.3, 0.3);
	glutSolidTorus(0.05, 0.005, 10, 10);

	glColor3f(0.6, 0.6, 0.6);
	GLUquadricObj *qobj = gluNewQuadric();
    gluQuadricDrawStyle( qobj, GLU_FILL );
    gluQuadricNormals( qobj, GLU_SMOOTH );
    gluQuadricOrientation( qobj, GLU_OUTSIDE );
    gluQuadricTexture( qobj, GL_FALSE );
	gluCylinder(qobj, 1.0, 1.0, 0.3, 20, 8);
	gluDisk(qobj, 0.7, 1.0, 20, 3);

	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glColor3f(0.3, 0.4, 0.3);
	glBegin(GL_POLYGON);
		glVertex3f(-3.0, -0.3,  3.0);
		glVertex3f( 3.0, -0.3,  3.0);
		glVertex3f( 3.0, -0.3, -3.0);
		glVertex3f(-3.0, -0.3, -3.0);
	glEnd();

	glFlush();
	glutSwapBuffers();
}

void Reshape(int x, int y) {
	glMatrixMode(GL_PROJECTION);  
	glLoadIdentity();
	gluPerspective(40.0,(GLdouble)x/(GLdouble)y,0.5,20.0);

	glMatrixMode(GL_MODELVIEW);
	glViewport(0,0,x,y);
	glPointSize(GLfloat(x)/300.0);
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
               if (state == GLUT_DOWN) 
				   DoUpdateScene = !DoUpdateScene;
               break;
          default:
               break;
     }

     glutPostRedisplay();
}

void MyMotion(int x, int y) {
     ptCurrentMousePosit.x = x;
     ptCurrentMousePosit.y = y;

     if( bMousing ) {
          g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
          g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
     }

     ptLastMousePosit.x = ptCurrentMousePosit.x;
     ptLastMousePosit.y = ptCurrentMousePosit.y;

     glutPostRedisplay();
}

int main(int argc, char **argv) {	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800,600);
	glutCreateWindow("Particle - Fountain");

	InitFountain();
	
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMotion);
	//glutIdleFunc(Display);

	glutMainLoop();
	return 0;
}