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

/*
 * dviewer_engine.c :
 */

#include <math.h>
#include "dviewer_engine.h"

/*
 * Draw ortogonal axis in RGB colors
 */
void
d_viewer_draw_reference_frame ( GLfloat     axisLength,
                                GLfloat     axisRadius )
{
    glPushMatrix();
    GLUquadricObj *pObj;
    pObj = gluNewQuadric();
    gluQuadricDrawStyle(pObj, GLU_FILL);
    gluQuadricNormals(pObj, GLU_SMOOTH);
    gluQuadricOrientation(pObj, GLU_OUTSIDE);
    glColor3f(0.7f, 0.2f, 0.2f);
    gluSphere(pObj, 1.5 * axisRadius, 20, 10);
    // X axis
    glRotatef(90, 0.0, 1.0, 0.0);
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    gluCylinder ( pObj,
                  axisRadius,
                  axisRadius,
                  axisLength,
                  30,
                  50 );
    glTranslatef(0.0, 0.0, axisLength);
    glRotatef(180.0, 1.0, 0.0, 0.0);
    gluDisk     ( pObj,
                  axisRadius,
                  2.0 * axisRadius,
                  10,
                  10 );
    glRotatef(180.0, 1.0, 0.0, 0.0);
    gluCylinder ( pObj,
                  2.0 * axisRadius,
                  0.0,
                  2.0 * axisRadius,
                  10,
                  10 );
    glPopMatrix();

    // Y axis
    glRotatef(-90.0, 1.0, 0.0, 0.0);
    glColor3f(0.0f, 1.0f, 0.0f);
    glPushMatrix();
    gluCylinder ( pObj,
                  axisRadius,
                  axisRadius,
                  axisLength,
                  30,
                  50 );
    glTranslatef(0.0, 0.0, axisLength);
    glRotatef(180.0, 1.0, 0.0, 0.0);
    gluDisk     ( pObj,
                  axisRadius,
                  2.0 * axisRadius,
                  10,
                  10 );
    glRotatef(180.0, 1.0, 0.0, 0.0);
    gluCylinder ( pObj,
                  2.0 * axisRadius,
                  0.0,
                  2.0 * axisRadius,
                  10,
                  10 );
    glPopMatrix();

    // Z axis
    glRotatef(-90, 0.0, 1.0, 0.0);
    glColor3f(0.0f, 0.0f, 1.0f);
    glPushMatrix();
    gluCylinder ( pObj,
                  axisRadius,
                  axisRadius,
                  axisLength,
                  30,
                  50 );
    glTranslatef(0.0, 0.0, axisLength);
    glRotatef(180.0, 1.0, 0.0, 0.0);
    gluDisk     ( pObj,
                  axisRadius,
                  2.0 * axisRadius,
                  10,
                  10 );
    glRotatef(180.0, 1.0, 0.0, 0.0);
    gluCylinder ( pObj,
                  2.0 * axisRadius,
                  0.0,
                  2.0 * axisRadius,
                  10,
                  10 );
    glPopMatrix();

    gluDeleteQuadric(pObj);
    glPopMatrix();
}

/*
 * Draw a platform in the current position and orientation
 * Should be put inside a list, we are doing some transformations here
 */
void
d_viewer_draw_platform ( GLfloat    length,
                         GLfloat    thick,
                         GLfloat    jdiam )
{
    // Save transformation
    glPushMatrix();

    GLfloat x = length - jdiam / 2.0;
    GLfloat y = jdiam / 2.0 * tan (30.0 / 180.0 * M_PI);
    GLfloat z = 0.5 * thick;

    // Small sides
    glBegin(GL_QUADS);
        {
            int i;
            for (i = 0; i < 3; i++) {
                GLfloat phi = ((GLfloat) i ) * 120.0 / 180.0 * M_PI;
                GLfloat X = x * cos (phi) - y * sin (phi);
                GLfloat Y = x * sin (phi) + y * cos (phi);
                glVertex3f(X, Y, -z);
                glVertex3f(X, Y,  z);
                X = x * cos (phi) + y * sin (phi);
                Y = x * sin (phi) - y * cos (phi);
                glVertex3f(X, Y,  z);
                glVertex3f(X, Y, -z);
            }
        }
    glEnd();

    // Big sides
    glBegin(GL_QUADS);
        {
            int i;
            for (i = 0; i < 3; i++) {
                GLfloat phi = ((GLfloat) i ) * 120.0 / 180.0 * M_PI;
                GLfloat X = x * cos (phi) - y * sin (phi);
                GLfloat Y = x * sin (phi) + y * cos (phi);
                glVertex3f(X, Y,  z);
                glVertex3f(X, Y, -z);
                phi = ((GLfloat) i + 1) * 120.0 / 180.0 * M_PI;
                X = x * cos (phi) + y * sin (phi);
                Y = x * sin (phi) - y * cos (phi);
                glVertex3f(X, Y, -z);
                glVertex3f(X, Y,  z);
            }
        }
    glEnd();
    // Move upward by half the thickness
    glTranslatef(0.0, 0.0, 0.5 * thick);

    // Use polygons

    // UP
    glBegin(GL_POLYGON);
        {
            int i;
            for (i = 0; i < 3; i++) {
                GLfloat phi = ((GLfloat) i ) * 120.0 / 180.0 * M_PI;
                GLfloat X = x * cos (phi) + y * sin (phi);
                GLfloat Y = x * sin (phi) - y * cos (phi);
                glVertex2f( X, Y );
                X = x * cos (phi) - y * sin (phi);
                Y = x * sin (phi) + y * cos (phi);
                glVertex2f( X, Y );
            }
        }
    glEnd();

    // DOWN
    glRotatef(180.0, 1.0, 0.0, 0.0);
    glTranslatef(0.0, 0.0, thick );
    glBegin(GL_POLYGON);
        {
            int i;
            for (i = 0; i < 3; i++) {
                GLfloat phi = ((GLfloat) i ) * 120.0 / 180.0 * M_PI;
                GLfloat X = x * cos (phi) + y * sin (phi);
                GLfloat Y = x * sin (phi) - y * cos (phi);
                glVertex2f( X, Y );
                X = x * cos (phi) - y * sin (phi);
                Y = x * sin (phi) + y * cos (phi);
                glVertex2f( X, Y );
            }
        }
    glEnd();
    // Reset transformation
    glPopMatrix();
}


void
d_viewer_draw_upper_limb ( GLfloat  length,
                           GLfloat  jdiam )
{
    glPushMatrix();

    GLUquadricObj *pObj;
    pObj = gluNewQuadric();
    gluQuadricDrawStyle(pObj, GLU_FILL);
    gluQuadricNormals(pObj, GLU_SMOOTH);
    gluQuadricOrientation(pObj, GLU_OUTSIDE);

    gluSphere(pObj, jdiam / 2.0, 20, 30);
    gluCylinder(pObj, jdiam / 4.0, jdiam / 4.0, length, 20, 30);
    glTranslatef(0.0, 0.0, length);
    gluSphere(pObj, jdiam / 2.0, 20, 30);

    gluDeleteQuadric(pObj);
    glPopMatrix();
}

void
d_viewer_draw_limb ( GLfloat  length,
                     GLfloat  jdiam )
{
    glPushMatrix();

    GLUquadricObj *pObj;
    pObj = gluNewQuadric();
    gluQuadricDrawStyle(pObj, GLU_FILL);
    gluQuadricNormals(pObj, GLU_SMOOTH);
    gluQuadricOrientation(pObj, GLU_OUTSIDE);

    gluSphere(pObj, jdiam / 2.0, 20, 30);
    gluCylinder(pObj, jdiam / 4.0, jdiam / 4.0, length, 20, 30);
    glTranslatef(0.0, 0.0, length);
    gluSphere(pObj, jdiam / 2.0, 20, 30);

    gluDeleteQuadric(pObj);
    glPopMatrix();
}

void
d_viewer_draw_arm ( GLfloat a,
                    GLfloat b,
                    GLfloat theta1,
                    GLfloat theta2,
                    GLfloat theta3,
                    GLfloat jdiam )
{
    glPushMatrix();

    // Will assume orientation on the X axis for theta1
    GLfloat t1 = theta1 / M_PI * 180.0;
    GLfloat t2 = theta2 / M_PI * 180.0;
    GLfloat t3 = theta3 / M_PI * 180.0;

    glRotatef(t1 - 90.0 , 0.0, -1.0, 0.0);
    d_viewer_draw_limb(a, jdiam);
    glTranslatef(0.0, 0.0, a);
    //Draw a little joint

    glPushMatrix();
    glTranslatef(0.0, jdiam, 0.0);
    glRotatef(t2, 0.0, -1.0, 0.0);
    glRotatef(t3 - 90.0, 1.0, 0.0, 0.0);
    d_viewer_draw_limb(b, jdiam);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, - jdiam, 0.0);
    glRotatef(t2, 0.0, -1.0, 0.0);
    glRotatef(t3 - 90.0, 1.0, 0.0, 0.0);
    d_viewer_draw_limb(b, jdiam);
    glPopMatrix();

    //Draw a little joint
//    glRotatef(t2, 0.0, 1.0, 0.0);
//    glRotatef(t3 - 90.0, 1.0, 0.0, 0.0);
//    glTranslatef(0.0, 0.0, b);
//    glRotatef(90.0 - t3, 1.0, 0.0, 0.0);

    glPopMatrix();
}