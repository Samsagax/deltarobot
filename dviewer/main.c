/*
 * Copyright (c) 2012, Joaquín Ignacio Aramendía
 * Author: Joaquín Ignacio Aramendía <samsagax [at] gmail [dot] com>
 *
 * This file is part of PROJECTNAME.
 *
 * PROJECTNAME is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PROJECTNAME is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PROJECTNAME. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <GL/glut.h>
#include <dsim/dsim_calc.h>

static const DGeometry robot = {40.0f, 50.0f, 20.0f, 30.0f};
static DPos point = { 0, 0, 50 };

static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
static GLfloat zRot = 0.0f;

void
arrowControl (int key, int x, int y)
{
    switch (key){
        case GLUT_KEY_UP:
            point.x += 1.0f;
            break;
        case GLUT_KEY_DOWN:
            point.x -= 1.0f;
            break;
        case GLUT_KEY_LEFT:
            point.y += 1.0f;
            break;
        case GLUT_KEY_RIGHT:
            point.y -= 1.0f;
            break;
        case GLUT_KEY_PAGE_UP:
            point.z += 1.0f;
            break;
        case GLUT_KEY_PAGE_DOWN:
            point.z -= 1.0f;
            break;
        case GLUT_KEY_HOME:
            point.x = 0.0f;
            point.y = 0.0f;
            point.z = 40.0f;
            break;
    }
    glutPostRedisplay();
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			exit(0);
        case '0':
            xRot = 0.0f;
            yRot = 0.0f;
            zRot = 0.0f;
            break;
        case 'a':
            yRot -= 5.0f;
            break;
        case 'd':
            yRot += 5.0f;
            break;
        case 'w':
            xRot -= 5.0f;
            break;
        case 's':
            xRot += 5.0f;
            break;
        case 'q':
            zRot -= 5.0f;
            break;
        case 'e':
            zRot += 5.0f;
            break;
	}
    glutPostRedisplay();
}

void handleResize(GLsizei w, GLsizei h) {
    GLfloat fAspect;

    // Prevet a division by zero
    if (h == 0) {
        h = 1;
    }

    // Set viewport to window dimensions
	glViewport(0, 0, w, h);

    fAspect = (GLfloat)w / (GLfloat)h;

    // Reset coordinate system
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    // Project with perspective
    gluPerspective(30.0f, fAspect, 1.0f, 400.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void initRendering() {
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glLineWidth(2.0f);

}

void
drawUnitAxes() {
    // Draw XYZ axis
    glColor3f(0.8f, 0.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(10.0f, 0.0f, 0.0f);
    glEnd();
    glColor3f(0.0f, 0.8f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 10.0f, 0.0f);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.8f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 10.0f);
    glEnd();
}

void
drawLimb( GLfloat a,
          GLfloat b,
          GLfloat t1,
          GLfloat t2,
          GLfloat t3) {
    // TODO:Draw from current transformation for efficiency
    //      and less error-prone
    GLfloat cos1 = cosf(t1);
    GLfloat sin1 = sinf(t1);
    GLfloat cos3 = cosf(t3);
    GLfloat sin3 = sinf(t3);
    GLfloat cos12 = cosf(t1 + t2);
    GLfloat sin12 = sinf(t1 + t2);

    GLfloat pb[] = { a * cos1,
                    0.0f,
                    a * sin1 };
    GLfloat pc[] = { a * cos1 + b * sin3 * cos12,
                    b * cos3,
                    a * sin1 + b * sin3 * sin12 };

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(pb[0], pb[1], pb[2]);
        glVertex3f(pb[0], pb[1], pb[2]);
        glVertex3f(pc[0], pc[1], pc[2]);
    glEnd();
}

void
drawPlatform( GLfloat l )
{
    glDisable(GL_CULL_FACE);
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    {
        int i;
        for (i = 0; i < 3; i++) {
            float phi = i * M_PI * 2.0f / 3.0f;
            glVertex3f(l * cosf(phi), l * sinf(phi), 0.0f);
        }
    }
    glEnd();
    glEnable(GL_CULL_FACE);
}

void
drawRobot (const DGeometry *robot, const DPos* pos) {
    DAxes ax;
    DExtendedAxes extax;
    solve_inverse(robot, pos, &ax, &extax);
    drawPlatform(robot->r);
    {
        int i;
        for ( i = 0; i < 3 ; i++ ) {
            glPushMatrix();
            glRotatef(i*120.0f, 0.0f, 0.0f, 1.0f);
            glTranslatef(robot->r, 0.0f, 0.0f);
            GLfloat t1 = extax.limb[i].rax[0];
            GLfloat t2 = extax.limb[i].rax[1];
            GLfloat t3 = extax.limb[i].rax[2];
            printf("Limb %i: t1: %f; t2: %f; t3: %f\n", i, t1, t2, t3);
            drawLimb(robot->a, robot->b, t1, t2, t3);
            glPopMatrix();
        }
    }
    glPushMatrix();
    glTranslatef(pos->x, pos->y, pos->z);
    drawPlatform(robot->h);
    drawUnitAxes();
    glPopMatrix();
    return;
}


void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glPushMatrix();
    // Move object back and do in place rotation
    glTranslatef(0.0f, -50.0f, -300.0f);
    glRotatef(xRot - 90.0f +35.265f, 1.0f, 0.0f, 0.0f);
    glRotatef(yRot, 0.0f, 1.0f, 0.0f);
    glRotatef(zRot - 45.0f, 0.0f, 0.0f, 1.0f);

    // Draw
    drawRobot(&robot, &point);
    drawUnitAxes();

    /* Restore matrix state */
    glPopMatrix();
    glutSwapBuffers();
}

void updateScene(int value) {

}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowSize(400, 400);

	glutCreateWindow("Simple Delta robot");
	initRendering();

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
    glutSpecialFunc(arrowControl);
	glutReshapeFunc(handleResize);
    drawScene();
//	glutTimerFunc(25, updateScene, 0);

	glutMainLoop();
	return 0;
}

