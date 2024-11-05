//  Use the accumulation buffer to do full-scene antialiasing on a scene with orthographic parallel projection.

#include <Windows.h>
#include <stdlib.h>
#include <glut.h>
#include <glext.h>

typedef struct 
{
	GLfloat x, y;
} jitter_point;

// 2 jitter points 
jitter_point j2[] =
{
	{ 0.246490,  0.249999},	{-0.246490, -0.249999}
};

// 3 jitter points 
jitter_point j3[] =
{
	{-0.373411, -0.250550},	{ 0.256263,  0.368119},	{ 0.117148, -0.117570}
};

// 4 jitter points 
jitter_point j4[] =
{
	{-0.208147,  0.353730},	{ 0.203849, -0.353780},	{-0.292626, -0.149945},
	{ 0.296924,  0.149994}
};

// 8 jitter points 
jitter_point j8[] =
{
	{-0.334818,  0.435331},	{ 0.286438, -0.393495},	{ 0.459462,  0.141540},
	{-0.414498, -0.192829},	{-0.183790,  0.082102},	{-0.079263, -0.317383},
	{ 0.102254,  0.299133},	{ 0.164216, -0.054399}
};

// 15 jitter points 
jitter_point j15[] =
{
	{ 0.285561,  0.188437},	{ 0.360176, -0.065688},	{-0.111751,  0.275019},
	{-0.055918, -0.215197},	{-0.080231, -0.470965},	{ 0.138721,  0.409168},
	{ 0.384120,  0.458500},	{-0.454968,  0.134088},	{ 0.179271, -0.331196},
	{-0.307049, -0.364927},	{ 0.105354, -0.010099},	{-0.154180,  0.021794},
	{-0.370135, -0.116425},	{ 0.451636, -0.300013},	{-0.370610,  0.387504}
};

// 24 jitter points
jitter_point j24[] =
{
	{ 0.030245,  0.136384},	{ 0.018865, -0.348867},	{-0.350114, -0.472309},
	{ 0.222181,  0.149524},	{-0.393670, -0.266873},	{ 0.404568,  0.230436},
	{ 0.098381,  0.465337},	{ 0.462671,  0.442116},	{ 0.400373, -0.212720},
	{-0.409988,  0.263345},	{-0.115878, -0.001981},	{ 0.348425, -0.009237},
	{-0.464016,  0.066467},	{-0.138674, -0.468006},	{ 0.144932, -0.022780},
	{-0.250195,  0.150161},	{-0.181400, -0.264219},	{ 0.196097, -0.234139},
	{-0.311082, -0.078815},	{ 0.268379,  0.366778},	{-0.040601,  0.327109},
	{-0.234392,  0.354659},	{-0.003102, -0.154402},	{ 0.297997, -0.417965}
};

// 66 jitter points
jitter_point j66[] =
{
	{ 0.266377, -0.218171},	{-0.170919, -0.429368},	{ 0.047356, -0.387135},
	{-0.430063,  0.363413},	{-0.221638, -0.313768},	{ 0.124758, -0.197109},
	{-0.400021,  0.482195},	{ 0.247882,  0.152010},	{-0.286709, -0.470214},
	{-0.426790,  0.004977},	{-0.361249, -0.104549},	{-0.040643,  0.123453},
	{-0.189296,  0.438963},	{-0.453521, -0.299889},	{ 0.408216, -0.457699},
	{ 0.328973, -0.101914},	{-0.055540, -0.477952},	{ 0.194421,  0.453510},
	{ 0.404051,  0.224974},	{ 0.310136,  0.419700},	{-0.021743,  0.403898},
	{-0.466210,  0.248839},	{ 0.341369,  0.081490},	{ 0.124156, -0.016859},
	{-0.461321, -0.176661},	{ 0.013210,  0.234401},	{ 0.174258, -0.311854},
	{ 0.294061,  0.263364},	{-0.114836,  0.328189},	{ 0.041206, -0.106205},
	{ 0.079227,  0.345021},	{-0.109319, -0.242380},	{ 0.425005, -0.332397},
	{ 0.009146,  0.015098},	{-0.339084, -0.355707},	{-0.224596, -0.189548},
	{ 0.083475,  0.117028},	{ 0.295962, -0.334699},	{ 0.452998,  0.025397},
	{ 0.206511, -0.104668},	{ 0.447544, -0.096004},	{-0.108006, -0.002471},
	{-0.380810,  0.130036},	{-0.242440,  0.186934},	{-0.200363,  0.070863},
	{-0.344844, -0.230814},	{ 0.408660,  0.345826},	{-0.233016,  0.305203},
	{ 0.158475, -0.430762},	{ 0.486972,  0.139163},	{-0.301610,  0.009319},
	{ 0.282245, -0.458671},	{ 0.482046,  0.443890},	{-0.121527,  0.210223},
	{-0.477606, -0.424878},	{-0.083941, -0.121440},	{-0.345773,  0.253779},
	{ 0.234646,  0.034549},	{ 0.394102, -0.210901},	{-0.312571,  0.397656},
	{ 0.200906,  0.333293},	{ 0.018703, -0.261792},	{-0.209349, -0.065383},
	{ 0.076248,  0.478538},	{-0.073036, -0.355064},	{ 0.145087,  0.221726}
};

static POINT ptLastMousePosit;
static POINT ptCurrentMousePosit;
static bool bMousing;

float g_fDistance = -7.0f;
float g_fSpinX    = 0.0f;
float g_fSpinY    = 0.0f;

void MyInit() {
   GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
   GLfloat lm_ambient[] = { 0.2, 0.2, 0.2, 1.0 };

   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
   glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
   glLightfv(GL_LIGHT0, GL_POSITION, light_position);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lm_ambient);
    
   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   glShadeModel (GL_FLAT);

   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClearAccum(0.0, 0.0, 0.0, 0.0);
}

void displayObjects() {
	GLfloat torus_diffuse[] = { 0.7, 0.7, 0.0, 1.0 };
	GLfloat cube_diffuse[] = { 0.0, 0.7, 0.7, 1.0 };
	GLfloat sphere_diffuse[] = { 0.7, 0.0, 0.7, 1.0 };
	GLfloat octa_diffuse[] = { 0.7, 0.4, 0.4, 1.0 };
    
	glPushMatrix ();
		glRotatef (30.0, 1.0, 0.0, 0.0);

		glPushMatrix ();
			glTranslatef (-0.80, 0.35, 0.0); 
			glRotatef (100.0, 1.0, 0.0, 0.0);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, torus_diffuse);
			glutSolidTorus (0.275, 0.85, 16, 16);
		glPopMatrix ();

		glPushMatrix ();
			glTranslatef (-0.75, -0.50, 0.0); 
			glRotatef (45.0, 0.0, 0.0, 1.0);
			glRotatef (45.0, 1.0, 0.0, 0.0);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, cube_diffuse);
			glutSolidCube (1.5);
		glPopMatrix ();

		glPushMatrix ();
			glTranslatef (0.75, 0.60, 0.0); 
			glRotatef (30.0, 1.0, 0.0, 0.0);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, sphere_diffuse);
			glutSolidSphere (1.0, 16, 16);
		glPopMatrix ();

		glPushMatrix ();
			glTranslatef (0.70, -0.90, 0.25); 
			glMaterialfv(GL_FRONT, GL_DIFFUSE, octa_diffuse);
			glutSolidOctahedron ();
		glPopMatrix ();
	glPopMatrix ();
}

#define ACSIZE	8

void MyDisplay() {
	GLint viewport[4];
    GLint  iMultiSample  = 0;
    GLint  iNumSamples = 0;

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glGetIntegerv (GL_VIEWPORT, viewport);
	glGetIntegerv (GL_SAMPLE_BUFFERS, &iMultiSample); 
    glGetIntegerv (GL_SAMPLES, &iNumSamples);

	glEnable(GL_MULTISAMPLE);
    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

	glTranslatef( 0.0f, 0.0f, g_fDistance );
    glRotatef( -g_fSpinY, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX, 0.0f, 1.0f, 0.0f );

	for (int jitter = 0; jitter < ACSIZE; jitter++) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPushMatrix ();
			//	Note that 4.5 is the distance in world space between left and right and bottom and top.
			//	This formula converts fractional pixel movement to world coordinates.
			glTranslatef (j8[jitter].x*4.5/viewport[2], j8[jitter].y*4.5/viewport[3], 0.0);
			displayObjects ();
		glPopMatrix ();

		glAccum(GL_ACCUM, 1.0/ACSIZE);
	}
	glAccum (GL_RETURN, 1.0);

	glFlush();
	glutSwapBuffers();
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

   if (w <= h) 
      glOrtho (-2.25, 2.25, -2.25*h/w, 2.25*h/w, -10.0, 10.0);
   else 
      glOrtho (-2.25*w/h, 2.25*w/h, -2.25, 2.25, -10.0, 10.0);

   //gluPerspective( 45.0f, w/h, 0.1f, 100.0f);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_ACCUM | GLUT_DEPTH);
	glutInitWindowSize (800, 600);
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Anti-Aliasing using Accumulation Buffer");
	MyInit();
	glutReshapeFunc(MyReshape);
	glutDisplayFunc(MyDisplay);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);
	glutSpecialFunc(MySpecial);

	glutMainLoop();
	return 0; 
}
