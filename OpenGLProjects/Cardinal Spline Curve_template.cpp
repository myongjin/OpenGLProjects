#include <vector>

#include <gl/glut.h>
#include <vector>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;


bool animate = true;
const int startwinsize = 640;
const double vertsize = 10.;		// Size of control pts (pixels)
const double curvewid = 4.;			// Width of drawn curve (pixels)
const double polywid = 2.;			// Width of control polygon (pixels)
const int numcurvesegments = 100;	// Number of line segs per curve segment
const double tensionchg = 0.01;		// Amt to chg tension on single keypress

vector<vec3> verts;	// Vector of 2-D control pts

bool currentlymoving;				// Are we dragging a control pt?
int selectedvert;					// Index of dragged pt; valid if currentlymoving
vec3 savevert;			// Start coords of dragged control pt
int savemx, savemy;					// Start mouse pos when dragging (pixels)

// Draws cardinal spline, given vector of control points, number of line segments to draw for each spline segment, and tension
// First and last segments used 2nd deriv = 0 constraint on first & last control pt.
vec3 computeInterpolatedPoint(vector<vec3> points, float t)
{
	vec3 result(0, 0, 0);
	if (points.size() == 4) {

		return result;
	}
	else if (points.size() == 3)
	{

		return result;
	}
	else {
		return result;
	}
}


void drawCardinalSpline(vector<vec3>& points, int segment){
	//점이 3개 이하인 경우
	if (points.size() < 4)
	{
		if (points.size() == 3)
		{
			glBegin(GL_LINE_STRIP);
		
			glEnd();
		}
		else if(points.size() == 2) {
			glBegin(GL_LINE_STRIP);
		
			glEnd();
		}
	}
	//점이 4개 이상인 경우
	else {
		
		//처음은 2차 함수
		glBegin(GL_LINE_STRIP);
		glColor3f(1, 0, 0);
		
		glEnd();
		
		//중간의 3차 함수로 그림
		
		//마지막도 2차 함수
		glBegin(GL_LINE_STRIP);
		glColor3f(1, 0, 0);

		glEnd();

		
	}
}

void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    // Draw control points
	glPointSize(vertsize);
    glBegin(GL_POINTS);
		for (int i=0; i<verts.size(); ++i) {
			if (i==selectedvert)
				glColor3d(1., 0., 0.);
            else
                glColor3d(0., 0., 0.);

            glVertex3f(verts[i][0], verts[i][1], verts[i][2]);
		}
    glEnd();

    // Draw curve
    glColor3d(0., 0., 1.);
    glPointSize(curvewid);
    glLineWidth(curvewid);
    drawCardinalSpline(verts, numcurvesegments);

    glutSwapBuffers();
}

void MyMouse(int button, int state, int x, int y) {
	switch (button) {
		case GLUT_LEFT_BUTTON:
			if (state == GLUT_DOWN) {
				int mousex = x;
				int mousey = startwinsize-y;

				// Check if clicked on a vert
				int i;
				for (i = 0; i < verts.size(); ++i) {
					int slop = (vertsize/2)+2;
					if (mousex >= verts[i][0]-slop
						&& mousex <= verts[i][0]+slop
						&& mousey >= verts[i][1]-slop
						&& mousey <= verts[i][1]+slop) break;
				}

				// If did not click on a vert, make a new one
				if (i == verts.size()) {
					verts.push_back(vec3(mousex, mousey, 0));
				}

				// Select vert & get ready for moving
				selectedvert = i;
				savemx = mousex; savemy = mousey;
				savevert = verts[i];
				currentlymoving = true;
			} else 
				currentlymoving = false;
			break;
		case GLUT_RIGHT_BUTTON:
			if (state == GLUT_DOWN) {
				if (verts.size() > 0) {
					verts.pop_back();
					selectedvert = -1;
					currentlymoving = false;
				}					
			}
			break;
		default:
			break;
	}

    glutPostRedisplay();
}

void MyMotion(int x, int y) {
    if (!currentlymoving) return;

    int mousex = x;
    int mousey = startwinsize-y;

    verts[selectedvert][0] = savevert[0]+mousex-savemx;
    verts[selectedvert][1] = savevert[1]+mousey-savemy;

    glutPostRedisplay();
}

void MyReshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(0., (double)w, (double)startwinsize - h, (double)startwinsize, -10, 10);
}

void MyInit() {
    currentlymoving = false;
    selectedvert = -1;
    glClearColor(1.0, 1.0, 1.0, 0.0);
}


int main(int argc, char ** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Cardinal Spline Curve");
    MyInit();
    glutDisplayFunc(MyDisplay);
    glutReshapeFunc(MyReshape);
    glutMouseFunc(MyMouse);
    glutMotionFunc(MyMotion);

    glutMainLoop();
    return 0;
}

