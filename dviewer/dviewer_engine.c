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
#include <stdio.h>
#include "dviewer_engine.h"

/*
 * Draw ortogonal axis in RGB colors
 */
void
d_viewer_draw_reference_frame ( GLfloat     axisLength,
                                GLfloat     axisRadius,
                                GLenum      drawStyle )
{
    glPushMatrix();
    GLUquadricObj *pObj;
    pObj = gluNewQuadric();
    gluQuadricDrawStyle(pObj, drawStyle);
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
                         GLfloat    jdiam,
                         GLenum     drawStyle )
{
    // Save transformation
    glPushMatrix();

    // Move upward by half the thickness
    glTranslatef(0.0, 0.0, 0.5 * thick);

    // Use polygons
    glColor3f(1.0, 0.3, 0.0);
    // UP
    GLfloat x = length - jdiam / 2.0;
    GLfloat y = jdiam / 2.0 * tan (30.0 / 180.0 * M_PI);
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

    // SIDES
    {
        int i;
        for (i = 0; i < 3; i++) {
            ;
        }
    }

    glPopMatrix();
    glPushMatrix();
    // DOWN
    glRotatef(180.0, 1.0, 0.0, 0.0);
    glTranslatef(0.0, 0.0, thick / 2.0);
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
