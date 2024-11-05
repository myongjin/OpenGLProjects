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
vector<vec3> curv; //For animation
int index = 0; //For animation
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
		result += (float)(0.5 * (-t * t * t + 2 * t * t - t)) * points[0];
		result += (float)(0.5 * (3* t * t * t - 5 * t * t + 2)) * points[1];
		result += (float)(0.5 * (-3* t * t * t + 4 * t * t + t)) * points[2];
		result += (float)(0.5 * (t * t * t - t * t )) * points[3];
		return result;
	}
	else if (points.size() == 3)
	{
		result += (float)( 2 * t * t -3 * t + 1 ) * points[0];
		result += (float)(-4 * t * t + 4* t) * points[1];
		result += (float)( 2 * t * t - t ) * points[2];
		return result;
	}
	else {
		return result;
	}
}


void drawCardinalSpline(vector<vec3>& points, int segment){
	if (animate) curv.clear();
	//점이 3개 이하인 경우
	if (points.size() < 4)
	{
		if (points.size() == 3)
		{
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i <= segment; i++)
			{
				float t = (float)i / (float)segment;
				vec3 p = computeInterpolatedPoint(points, t);
				glVertex3f(p[0], p[1], p[2]);
				if (animate) curv.push_back(p);

			}
			glEnd();
		}
		else if(points.size() == 2) {
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < points.size(); i++)
			{
				vec3 p = points[i];
				glVertex3f(p[0], p[1], p[2]);
				if (animate) curv.push_back(p);
			}
			glEnd();
		}
	}
	//점이 4개 이상인 경우
	else {
		
		//처음은 2차 함수
		vector<vec3> tempPoints;
		tempPoints.clear();
		for (int i = 0; i < 3; i++)
		{
			tempPoints.push_back(points[i]);
		}
		glBegin(GL_LINE_STRIP);
		glColor3f(1, 0, 0);
		for (int i = 0; i <= segment/2; i++)
		{
			float t = (float)i / (float)segment;
			vec3 p = computeInterpolatedPoint(tempPoints, t);
			glVertex3f(p[0], p[1], p[2]);
			if (animate) curv.push_back(p);
		}
		glEnd();
		
		//중간의 3차 함수로 그림
		int nbSection = points.size() - 3;
		for (int sec = 0; sec < nbSection; sec++)
		{
			tempPoints.clear();
			for (int i = 0; i < 4; i++)
			{
				tempPoints.push_back(points[sec + i]);
			}
			glBegin(GL_LINE_STRIP);
			glColor3f(0, 0, 1);
			for (int i = 0; i <= segment; i++)
			{
				float t = (float)i / (float)segment;
				vec3 p = computeInterpolatedPoint(tempPoints, t);
				glVertex3f(p[0], p[1], p[2]);
				if (animate) curv.push_back(p);
			}
			glEnd();
		}

		//마지막도 2차 함수
		tempPoints;
		tempPoints.clear();
		for (int i = 0; i < 3; i++)
		{
			tempPoints.push_back(points[points.size() - 3 + i]);
		}
		glBegin(GL_LINE_STRIP);
		glColor3f(1, 0, 0);
		for (int i = 0; i <= segment / 2; i++)
		{
			float t = (float)i / (float)segment;
			vec3 p = computeInterpolatedPoint(tempPoints, 0.5 + t);
			glVertex3f(p[0], p[1], p[2]);
			if (animate) curv.push_back(p);
		}
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

	glColor3d(1., 0., 0.);

	//Draw animated ball
	if (animate && curv.size() >3 )
	{
		glPushMatrix();
		glTranslatef(curv[index][0], curv[index][1], curv[index][2]);
		glutWireTorus(10,20, 3, 20);
		glPopMatrix();
	}
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

void MyTimer(int Value) {
	//update balls
	float dt = 0.1;
	if (++index >= curv.size()) index = 0;
	
	glutPostRedisplay();
	glutTimerFunc(30, MyTimer, 1);
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
	glutTimerFunc(30, MyTimer, 1);

    glutMainLoop();
    return 0;
}

