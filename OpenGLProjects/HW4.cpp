#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "Camera.h"

#include <vector>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>



void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);


    //Set projection;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //gluPerspective(fov, (float)windowWidth / (float)windowHeight, n, f);

    glOrtho(-2, 2, -2, 2, 1.f, 5.f);




    //Draw Scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //set Identity
    gluLookAt(1, 0, 5, 0, 0, 0, 0, 1, 0);


        
    glTranslatef(1.f, 2.f, 3.f);
    glutWireSphere(0.2, 20, 16);
    glPopMatrix();

    //Send it to buffer
    glFlush();
}



int main(int argc, char** argv) {
    //Create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(300, 300);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("OpenGL Sample Drawing");

    vec3 p(1, 2, 3);

    mat4 V = lookAt(vec3(1, 0, 5), vec3(0, 0, 0), vec3(0, 1, 0));

    //mat4 N = perspective(radians(100.f), 1.f, 1.f, 5.f);
    mat4 N = ortho(-2.f, 2.f, -2.f, 2.f, 1.f, 5.f);
    mat4 NV = N * V;
    vec4 convP = NV * vec4(p, 1);
    convP /= convP[3];

    convP[0] = (convP[0] + 1.f) / 2.f * 300;
    convP[1] = (1.f - convP[1]) / 2.f * 300;

    glutDisplayFunc(MyDisplay);
    //start mainloop
    glutMainLoop();
    return 0;
}