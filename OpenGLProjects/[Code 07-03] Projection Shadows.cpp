#include <windows.h>
#include <stdlib.h>
#include <glut.h>
#include <glu.h>
#include <gl.h>

// Spin control for view
float  g_fSpinX_L =   0.0f;
float  g_fSpinY_L = -10.0f;

// Spin control for teapot
float  g_fSpinX_R =   0.0f;
float  g_fSpinY_R =   0.0f;

float g_shadowMatrix[16];
float g_lightPosition[] = { 2.0f, 6.0f, 0.0f, 1.0f }; // World position of light source

struct Vertex
{
    float nx, ny, nz;
    float x, y, z;
};

Vertex g_floorQuad[] =
{
    { 0.0f, 1.0f,  0.0f,  -10.0f, 0.0f, -10.0f },
    { 0.0f, 1.0f,  0.0f,  -10.0f, 0.0f,  10.0f },
    { 0.0f, 1.0f,  0.0f,   10.0f, 0.0f,  10.0f },
    { 0.0f, 1.0f,  0.0f,   10.0f, 0.0f, -10.0f },
};

static POINT ptLastMousePosit_L;
static POINT ptCurrentMousePosit_L;
static bool  bMousing_L;
    
static POINT ptLastMousePosit_R;
static POINT ptCurrentMousePosit_R;
static bool  bMousing_R;

void MyReshape(int w, int h) {	
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w/(GLfloat)h, 1.0, 100.0);
}

void buildShadowMatrix( float fMatrix[16], float fLightPos[4], float fPlane[4] ) {
    float dotp;

    // Calculate the dot-product between the plane and the light's position
    dotp = fPlane[0] * fLightPos[0] + 
           fPlane[1] * fLightPos[1] + 
           fPlane[1] * fLightPos[2] + 
           fPlane[3] * fLightPos[3];

    // First column
    fMatrix[0]  = dotp - fLightPos[0] * fPlane[0];
    fMatrix[4]  = 0.0f - fLightPos[0] * fPlane[1];
    fMatrix[8]  = 0.0f - fLightPos[0] * fPlane[2];
    fMatrix[12] = 0.0f - fLightPos[0] * fPlane[3];

    // Second column
    fMatrix[1]  = 0.0f - fLightPos[1] * fPlane[0];
    fMatrix[5]  = dotp - fLightPos[1] * fPlane[1];
    fMatrix[9]  = 0.0f - fLightPos[1] * fPlane[2];
    fMatrix[13] = 0.0f - fLightPos[1] * fPlane[3];

    // Third column
    fMatrix[2]  = 0.0f - fLightPos[2] * fPlane[0];
    fMatrix[6]  = 0.0f - fLightPos[2] * fPlane[1];
    fMatrix[10] = dotp - fLightPos[2] * fPlane[2];
    fMatrix[14] = 0.0f - fLightPos[2] * fPlane[3];

    // Fourth column
    fMatrix[3]  = 0.0f - fLightPos[3] * fPlane[0];
    fMatrix[7]  = 0.0f - fLightPos[3] * fPlane[1];
    fMatrix[11] = 0.0f - fLightPos[3] * fPlane[2];
    fMatrix[15] = dotp - fLightPos[3] * fPlane[3];
}

//-----------------------------------------------------------------------------
// Name: findPlane()
// Desc: find the plane equation given 3 points
//-----------------------------------------------------------------------------
void findPlane( GLfloat plane[4], GLfloat v0[3], GLfloat v1[3], GLfloat v2[3] ) {
    GLfloat vec0[3], vec1[3];

    // Need 2 vectors to find cross product
    vec0[0] = v1[0] - v0[0];
    vec0[1] = v1[1] - v0[1];
    vec0[2] = v1[2] - v0[2];

    vec1[0] = v2[0] - v0[0];
    vec1[1] = v2[1] - v0[1];
    vec1[2] = v2[2] - v0[2];

    // Find cross product to get A, B, and C of plane equation
    plane[0] =   vec0[1] * vec1[2] - vec0[2] * vec1[1];
    plane[1] = -(vec0[0] * vec1[2] - vec0[2] * vec1[0]);
    plane[2] =   vec0[0] * vec1[1] - vec0[1] * vec1[0];

    plane[3] = -(plane[0] * v0[0] + plane[1] * v0[1] + plane[2] * v0[2]);
}

void renderFloor() {
    glColor3f( 1.0f, 1.0f, 1.0f );
    glInterleavedArrays( GL_N3F_V3F, 0, g_floorQuad );
    glDrawArrays( GL_QUADS, 0, 4 );
}

void render( void ) {
    //
    // Define the plane of the planar surface that we want to cast a shadow on...
    //

    GLfloat shadowPlane[4];
    GLfloat v0[3], v1[3], v2[3];

    // To define a plane that matches the floor, we need to 3 vertices from it
    v0[0] = g_floorQuad[0].x;
    v0[1] = g_floorQuad[0].y;
    v0[2] = g_floorQuad[0].z;

    v1[0] = g_floorQuad[1].x;
    v1[1] = g_floorQuad[1].y;
    v1[2] = g_floorQuad[1].z;

    v2[0] = g_floorQuad[2].x;
    v2[1] = g_floorQuad[2].y;
    v2[2] = g_floorQuad[2].z;

    findPlane( shadowPlane, v0, v1, v2 );

    //
    // Build a shadow matrix using the light's current position and the plane
    //

    buildShadowMatrix( g_shadowMatrix, g_lightPosition, shadowPlane );
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //
    // Place the view
    //
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef( 0.0f, -2.0f, -15.0f );
    glRotatef( -g_fSpinY_L, 1.0f, 0.0f, 0.0f );
    glRotatef( -g_fSpinX_L, 0.0f, 1.0f, 0.0f );

    //
    // Render the floor...
    //

    renderFloor();

    //
    // Create a shadow by rendering the teapot using the shadow matrix.
    //

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glColor3f(0.2f, 0.2f, 0.2f); // Shadow's color
    glPushMatrix();
    {
        glMultMatrixf((GLfloat *)g_shadowMatrix);

        // Teapot's position & orientation (needs to use the same transformations used to render the actual teapot)
        glTranslatef( 0.0f, 2.5f, 0.0f );
        glRotatef( -g_fSpinY_R, 1.0f, 0.0f, 0.0f );
        glRotatef( -g_fSpinX_R, 0.0f, 1.0f, 0.0f );
        glutSolidTeapot( 1.0 );
    }
    glPopMatrix();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    //
    // Render the light's position as a sphere...
    //

    glDisable( GL_LIGHTING );

    glPushMatrix();
    {
        // Place the light...
        glLightfv( GL_LIGHT0, GL_POSITION, g_lightPosition );

        // Place a sphere to represent the light
        glTranslatef( g_lightPosition[0], g_lightPosition[1], g_lightPosition[2] );

        glColor3f(1.0f, 1.0f, 0.5f);
        glutSolidSphere( 0.1, 8, 8 );
    }
    glPopMatrix();

    glEnable( GL_LIGHTING );

    //
    // Render normal teapot
    //

    glPushMatrix();
    {
        // Teapot's position & orientation
        glTranslatef( 0.0f, 2.5f, 0.0f );
        glRotatef( -g_fSpinY_R, 1.0f, 0.0f, 0.0f );
        glRotatef( -g_fSpinX_R, 0.0f, 1.0f, 0.0f );
        glutSolidTeapot( 1.0 );
    }
    glPopMatrix();

    glutSwapBuffers();
}

void init( void ) {
    glClearColor( 0.35f, 0.53f, 0.7f, 1.0f );
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    // Enable a single OpenGL light.
    float lightAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    float lightDiffuse[] = {1.0, 1.0, 1.0, 1.0}; 
    float lightSpecular[] = {1.0, 1.0, 1.0, 1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
}

void MySpecial(int key, int x, int y) {
	if(key == GLUT_KEY_UP) {
		g_lightPosition[1] += 0.1f;
	} else if(key == GLUT_KEY_DOWN) {
		g_lightPosition[1] -= 0.1f;
	} else if(key == GLUT_KEY_LEFT) {
		g_lightPosition[0] -= 0.1f;
	} else if(key == GLUT_KEY_RIGHT) {
		g_lightPosition[0] += 0.1f;
	}

	glutPostRedisplay();
}

void MyMouse(int button, int state, int x, int y) {
   switch (button) {
      case GLUT_LEFT_BUTTON:
         if (state == GLUT_DOWN) {
            ptLastMousePosit_L.x = ptCurrentMousePosit_L.x = x;
            ptLastMousePosit_L.y = ptCurrentMousePosit_L.y = y;
            bMousing_L = true;
		 } else 
			bMousing_L = false;
         break;
      case GLUT_MIDDLE_BUTTON:
      case GLUT_RIGHT_BUTTON:
         if (state == GLUT_DOWN) {
            ptLastMousePosit_R.x = ptCurrentMousePosit_R.x = x;
            ptLastMousePosit_R.y = ptCurrentMousePosit_R.y = y;
            bMousing_R = true;
		 } else 
			bMousing_R = false;
         break;
      default:
         break;
   }

   glutPostRedisplay();
}

void MyMotion(int x, int y) {
	ptCurrentMousePosit_L.x = x;
    ptCurrentMousePosit_L.y = y;
    ptCurrentMousePosit_R.x = x;
    ptCurrentMousePosit_R.y = y;

    if( bMousing_L )
    {
		g_fSpinX_L -= (ptCurrentMousePosit_L.x - ptLastMousePosit_L.x);
        g_fSpinY_L -= (ptCurrentMousePosit_L.y - ptLastMousePosit_L.y);
    }

    if( bMousing_R )
    {
        g_fSpinX_R -= (ptCurrentMousePosit_R.x - ptLastMousePosit_R.x);
        g_fSpinY_R -= (ptCurrentMousePosit_R.y - ptLastMousePosit_R.y);
    }
   
    ptLastMousePosit_L.x = ptCurrentMousePosit_L.x;
    ptLastMousePosit_L.y = ptCurrentMousePosit_L.y;
    ptLastMousePosit_R.x = ptCurrentMousePosit_R.x;
    ptLastMousePosit_R.y = ptCurrentMousePosit_R.y;

    glutPostRedisplay();
}

void main(int argc, char **argv){
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
    glutCreateWindow("Projection Shadows");
	init();
	glutDisplayFunc(render);
    glutReshapeFunc(MyReshape);
	glutSpecialFunc(MySpecial);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);

    glutMainLoop( );
}