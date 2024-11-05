
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>

void init(void) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
}
void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    //glBlendFunc(GL_ONE, GL_ZERO);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLES);
        glColor4f(1, 0, 0, 1);
        glVertex4f(0, 1, 0, 1);
        glVertex4f(-1, -1, 0, 1);
        glVertex4f(1, -1, 0, 1);
    glEnd();

    glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);

    glBegin(GL_QUADS);
        glColor4f(0, 0, 1, 0.5);
        glVertex4f(-0.8, 0.8, 0, 1);
        glVertex4f(0.8, 0.8, 0, 1);
        glVertex4f(0.8, -0.8, 0, 1);
        glVertex4f(-0.8, -0.8, 0, 1);
    glEnd();

    glutSwapBuffers();
}
void main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Blending");
    init();
    glutDisplayFunc(render);

    
    glutMainLoop();
}
