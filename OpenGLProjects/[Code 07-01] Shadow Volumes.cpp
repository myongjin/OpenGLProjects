#include <windows.h>
#include <stdlib.h>
#include <gl/glut.h>
#include <gl/glu.h>
#include <gl/gl.h>

// Spin control for view
float  g_fSpinX_L =   0.0f;
float  g_fSpinY_L = -10.0f;

// Spin control for shadow casting quad
float  g_fSpinX_R =   0.0f;
float  g_fSpinY_R =   0.0f;

bool  g_bRenderShadowVolume = false;
float g_fAmountOfExtrusion  = 5.0f;
float g_lightPosition[]     = { 2.0f, 6.0f, 0.0f, 1.0f }; // World position of light source

// GL_C3F_V3F
struct Vertex {
    float r, g, b;
    float x, y, z;
};

Vertex g_shadowCasterVerts[] = {
    { 1.0, 1.0, 1.0,  -1.0f, 2.5f, -1.0f },
    { 1.0, 1.0, 1.0,  -1.0f, 2.5f,  1.0f },
    { 1.0, 1.0, 1.0,   1.0f, 2.5f,  1.0f },
    { 1.0, 1.0, 1.0,   1.0f, 2.5f, -1.0f },
};

float g_shadowCasterNormal[] = { 0.0f, 1.0f, 0.0f };

struct ShadowCaster {
    Vertex *verts;        // Vertices of the actual shadow casting object
    float  *normal;       // A surface normal for lighting
    int     numVerts;     // Total number of vertices
    int     shadowVolume; // Display list for holding the shadow volume
};

ShadowCaster g_shadowCaster;

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

void extendVertex( float newVert[3], float lightPosit[3], Vertex vert, float ext ) {
    float lightDir[3];

    // Create a vector that points from the light's position to the original vertex.
    lightDir[0] = vert.x - lightPosit[0];
    lightDir[1] = vert.y - lightPosit[1];
    lightDir[2] = vert.z - lightPosit[2];

    // Then use that vector to extend the original vertex out to a new position.
    // The distance to extend or extrude the new vector is specified by t.
    newVert[0] = lightPosit[0] + lightDir[0] * ext;
    newVert[1] = lightPosit[1] + lightDir[1] * ext;
    newVert[2] = lightPosit[2] + lightDir[2] * ext;
}

void buildShadowVolume( ShadowCaster *caster, float lightPosit[3], float ext ) {
    if( caster->shadowVolume != -1 )
        glDeleteLists( caster->shadowVolume, 0 );

    caster->shadowVolume = glGenLists(1);

    glNewList( caster->shadowVolume, GL_COMPILE ); 
	{
        glDisable( GL_LIGHTING );

        glBegin( GL_QUADS );
        {
            glColor3f( 0.2f, 0.8f, 0.4f );

            float vExtended[3];

            //
            // For each vertex of the shadow casting object, find the edge 
            // that it helps define and extrude a quad out from that edge.
            //
            
            for( int i = 0; i < caster->numVerts; ++i )
            {
                // Define the edge we're currently working on extruding...
                int e0 = i;
                int e1 = i+1;

                // If the edge's second vertex is out of array range, 
                // place it back at 0
                if( e1 >= caster->numVerts )
                    e1 = 0;

                // v0 of our extruded quad will simply use the edge's first 
                // vertex or e0.
                glVertex3f( caster->verts[e0].x, 
                            caster->verts[e0].y, 
                            caster->verts[e0].z );

                // v1 of our quad is created by taking the edge's first 
                // vertex and extending it out by some amount.
                extendVertex( vExtended, lightPosit, caster->verts[e0], ext );
                glVertex3f( vExtended[0], vExtended[1], vExtended[2] );

                // v2 of our quad is created by taking the edge's second 
                // vertex and extending it out by some amount.
                extendVertex( vExtended, lightPosit, caster->verts[e1], ext );
                glVertex3f( vExtended[0], vExtended[1], vExtended[2] );

                // v3 of our extruded quad will simply use the edge's second 
                // vertex or e1.
                glVertex3f( caster->verts[e1].x, 
                            caster->verts[e1].y, 
                            caster->verts[e1].z );
            }
        }
        glEnd();

        glEnable( GL_LIGHTING );
    }
    glEndList();
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

    //
    // Set up the shadow caster...
    //

    g_shadowCaster.verts        = g_shadowCasterVerts;
    g_shadowCaster.normal       = g_shadowCasterNormal;
    g_shadowCaster.numVerts     = (sizeof(g_shadowCasterVerts) / sizeof(Vertex));
    g_shadowCaster.shadowVolume = -1;
}

void renderScene( void ) {
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

    glPushMatrix();
    {
        glBegin( GL_QUADS );
        {
            glNormal3f( 0.0f, 1.0f,  0.0f );
            glVertex3f(-10.0f, 0.0f, -10.0f );
            glVertex3f(-10.0f, 0.0f,  10.0f );
            glVertex3f( 10.0f, 0.0f,  10.0f );
            glVertex3f( 10.0f, 0.0f, -10.0f );
        }
        glEnd();
    }
    glPopMatrix();

    //
    // Render a teapot so we'll have something else to cast a shadow on  
    // besides the floor.
    //

    glPushMatrix();
    {
        glTranslatef( -2.0f, 0.8f, 0.0f );
        glRotatef( 180.0, 0.0f, 1.0f, 0.0f );
        glColor3f( 0.0f, 1.0f ,1.0f );
        glRotatef( -g_fSpinY_R, 1.0f, 0.0f, 0.0f );
        glRotatef( -g_fSpinX_R, 0.0f, 1.0f, 0.0f );
		glutSolidTeapot(1.0);
    }
    glPopMatrix();

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
		glutSolidSphere(0.1, 8, 8);
    }
    glPopMatrix();

    glEnable( GL_LIGHTING );

    //
    // Render the shadow caster (i.e. the quad)
    //

    glPushMatrix();
    {
        glBegin( GL_POLYGON );
        {
            glNormal3fv( g_shadowCaster.normal );

            for( int i = 0; i < g_shadowCaster.numVerts; ++i )
            {
                glVertex3f( g_shadowCaster.verts[i].x,
                            g_shadowCaster.verts[i].y,
                            g_shadowCaster.verts[i].z );
            }
        }
        glEnd();
    }
    glPopMatrix();
}

//-----------------------------------------------------------------------------
// Name: render()
// Desc: 
//-----------------------------------------------------------------------------
void render( void )
{
    //
    // Using the light's position, extend or extrude each vertex of the shadow
    // caster out by an amount specified by g_fAmountOfExtrusion.
    //

    buildShadowVolume( &g_shadowCaster, g_lightPosition, g_fAmountOfExtrusion );

    //
    // Prepare to render a new scene by clearing out all of the buffers.
    //

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    // Initialize the depth buffer by rendering the scene into it.
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    renderScene();

    //
    // Create the special shadow stencil...
    //

    // Set the appropriate states for creating a stencil for shadowing.
    glEnable( GL_CULL_FACE );
    glEnable( GL_STENCIL_TEST );
    glDepthMask( GL_FALSE );
    glStencilFunc( GL_ALWAYS, 0, 0 );
    
    // Render the shadow volume and increment the stencil every where a front
    // facing polygon is rendered.
    glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );
    glCullFace( GL_BACK );
    glCallList( g_shadowCaster.shadowVolume );

    // Render the shadow volume and decrement the stencil every where a back
    // facing polygon is rendered.
    glStencilOp( GL_KEEP, GL_KEEP, GL_DECR );
    glCullFace( GL_FRONT );
    glCallList( g_shadowCaster.shadowVolume );

    // When done, set the states back to something more typical.
    glDepthMask( GL_TRUE );
    glDepthFunc( GL_LEQUAL );
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
    glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
    glCullFace( GL_BACK );
    glDisable( GL_CULL_FACE );

    //
    // Render the shadowed part...
    //

    glStencilFunc( GL_EQUAL, 1, 1 );
    glDisable( GL_LIGHT0 );
    renderScene();

    //
    // Render the lit part...
    //

    glStencilFunc( GL_EQUAL, 0, 1 );
    glEnable( GL_LIGHT0 );
    renderScene();

    // When done, set the states back to something more typical.
    glDepthFunc( GL_LESS );
    glDisable( GL_STENCIL_TEST);

    if( g_bRenderShadowVolume )
    {
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glCallList( g_shadowCaster.shadowVolume );
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

	glutSwapBuffers();
}

void MySpecial(int key, int x, int y) {
	if(key == GLUT_KEY_F1)	{
		g_bRenderShadowVolume = !g_bRenderShadowVolume;
	} else if(key == GLUT_KEY_F2) {
		g_fAmountOfExtrusion += 0.1f;
	} else if(key == GLUT_KEY_F3) {
		g_fAmountOfExtrusion -= 0.1f;
	} else if(key == GLUT_KEY_UP) {
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
    glutCreateWindow("Shadow Volume");
	init();
	glutDisplayFunc(render);
    glutReshapeFunc(MyReshape);
	glutSpecialFunc(MySpecial);
	glutMouseFunc(MyMouse);
	glutMotionFunc(MyMotion);

    glutMainLoop( );
}