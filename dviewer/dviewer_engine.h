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
 * dviewer_engine.h : Small OpenGL engine to simulate a Delta robot
 */


#ifndef  DVIEWER_ENGINE_INC
#define  DVIEWER_ENGINE_INC

#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <dsim/dsim.h>

void    d_viewer_draw_reference_frame   (GLfloat   axisLength,
                                         GLfloat   axisRadius);

void    d_viewer_draw_platform          (GLfloat   platformLength,
                                         GLfloat   platformThickness,
                                         GLfloat   jointDiameter);

void    d_viewer_draw_limb              (GLfloat   length,
                                         GLfloat   jointDiameter);

void    d_viewer_draw_lower_limb        (GLfloat   length,
                                         GLfloat   jointDiameter);

void    d_viewer_draw_upper_limb        (GLfloat   length,
                                         GLfloat   jointDiameter);

void    d_viewer_draw_arm               (GLfloat   a,
                                         GLfloat   b,
                                         GLfloat   theta1,
                                         GLfloat   theta2,
                                         GLfloat   theta3,
                                         GLfloat   jointDiameter);

void    d_viewer_draw_robot_with_ext_axes (DGeometry    *geometry,
                                           DExtAxes     *extaxes);

void    d_viewer_draw_robot_at_pos      (DGeometry *geometry,
                                         DVector3  *pos );

#endif   /* ----- #ifndef DVIEWER_ENGINE_INC  ----- */
