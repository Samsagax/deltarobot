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
 * dviewer_viewport.h :
 */


#ifndef  DVIEWER_VIEWPORT_INC
#define  DVIEWER_VIEWPORT_INC

#include <gtk/gtk.h>
#include <gtk/gtkgl.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <math.h>

#include <dsim/dsim.h>
#include "dviewer_engine.h"

#define D_TYPE_VIEWPORT             (d_viewport_get_type())
#define D_VIEWPORT(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_VIEWPORT, DViewport))
#define D_IS_VIEWPORT(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_VIEWPORT))
#define D_VIEWPORT_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_VIEWPORT, DViewportClass))
#define D_IS_VIEWPORT_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_VIEWPORT))
#define D_VIEWPORT_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_VIEWPORT, DViewportClass))

typedef struct _DViewport DViewport;
typedef struct _DViewportClass DViewportClass;

struct _DViewport {
    GtkDrawingArea  parent;

    /* Robot Geometry */
    DGeometry       *geometry;
    DExtAxes        *extaxes;
    //DPos            *robot_pos;

    /* Mouse button pressed or 0 */
    guint8          button;

    /* Max FPS count */
    //guint           max_fps;            /* Not currently used */

    /* ID of timer or 0 if none */
    //guint32         timer;

    /* OpenGL context */
    GdkGLConfig     *glconfig;

    /* Define a sphere containing the geometry */
    DVector         *scene_center;
    gdouble         scene_distance;
    gdouble         polar_angle;
    gdouble         azimuth_angle;

    /* Define position for a light */
    //DVector         *light_position;

    /* Far and near clip distance */
    gdouble         near_clip;
    gdouble         far_clip;

    /* View angle or 0 for ortogonal view */
    gdouble         eye_angle;

    /* Euler view rotation angles in degrees */
    //DVector         *view_angles;

    /* Trackball for 3D rotation of camera */
    //gdouble         track_angle;
    //DVector         *track_axis;

    /* Zooming */
    //gdouble         zoom;
};

struct _DViewportClass {
    GtkDrawingAreaClass parent_class;
};

/* Methods */

GType       d_viewport_get_type                 (void);

GtkWidget*  d_viewport_new                      (DGeometry  *geometry);

GtkWidget*  d_viewport_new_with_pos             (DGeometry  *geometry,
                                                 DPos       *pos);

GtkWidget*  d_viewport_new_full                 (DGeometry  *geometry,
                                                 DExtAxes   *extaxes,
                                                 DVector    *scene_center,
                                                 gdouble    scene_distance,
                                                 gdouble    polar_angle,
                                                 gdouble    azimuth_angle,
                                                 gdouble    near_clip,
                                                 gdouble    far_clip,
                                                 gdouble    eye_angle);

DVector *   d_viewport_get_pos                  (const DViewport    *self);

void        d_viewport_set_pos                  (DViewport  *self,
                                                 DPos       *pos);

DExtAxes*   d_viewport_get_ext_axes             (const DViewport    *self);

void        d_viewport_set_ext_axes             (DViewport  *self,
                                                 DExtAxes   *extaxes);

DVector *   d_viewport_get_scene_center         (const DViewport    *self);

void        d_viewport_set_scene_center         (DViewport  *self,
                                                 DVector    *scene_center);

void        d_viewport_set_scene_center_xyz     (DViewport  *self,
                                                 gdouble    x,
                                                 gdouble    y,
                                                 gdouble    z);

gdouble     d_viewport_get_scene_distance       (const DViewport    *self);

void        d_viewport_set_scene_distance       (DViewport  *self,
                                                 gdouble    scene_distance);

gdouble     d_viewport_get_polar_angle          (const DViewport    *self);

void        d_viewport_set_polar_angle          (DViewport  *self,
                                                 gdouble    polar_angle);

gdouble     d_viewport_get_azimuth_angle        (const DViewport    *self);

void        d_viewport_set_azimuth_angle        (DViewport  *self,
                                                 gdouble    azimuth_angle);

gdouble     d_viewport_get_far_clip             (const DViewport    *self);

void        d_viewport_set_far_clip             (DViewport  *self,
                                                 gdouble    far_clip);

gdouble     d_viewport_get_near_clip            (const DViewport    *self);

void        d_viewport_set_near_clip            (DViewport  *self,
                                                 gdouble    near_clip);

gdouble     d_viewport_get_eye_angle            (const DViewport    *self);

void        d_viewport_set_eye_angle            (DViewport  *self,
                                                 gdouble    eye_angle);

void        d_viewport_configure_view           (DViewport  *self,
                                                 DVector    *scene_center,
                                                 gdouble    scene_distance,
                                                 gdouble    polar_angle,
                                                 gdouble    azimuth_angle,
                                                 gdouble    near_clip,
                                                 gdouble    far_clip,
                                                 gdouble    eye_angle);

void        d_viewport_queve_redraw             (DViewport  *self);

#endif   /* ----- #ifndef DVIEWER_VIEWPORT_INC  ----- */

