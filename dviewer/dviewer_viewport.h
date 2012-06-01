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

#include <dsim/dsim.h>

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
    DPos            *robot_pos;
    /* Mouse button pressed or 0 */
    guint8          button;

    /* Max FPS count */
    guint           max_fps;

    /* ID of timer or 0 if none */
    guint32         timer;

    /* OpenGL context */
    GdkGLConfig     *glconfig;

    /* Far and near clip distance */
    gdouble         near_clip;
    gdouble         far_clip;

    /* View angle or 0 for ortogonal view */
    gdouble         view_angle;

    /* X, Y and Z view rotation angles in degrees */
    gdouble         view_x_angle;
    gdouble         view_y_angle;
    gdouble         view_z_angle;

    /* Zooming */
    gdouble         zoom;
};

struct _DViewportClass {
    GtkDrawingAreaClass parent_class;
};

GType       d_viewport_get_type                 (void);
GtkWidget*  d_viewport_new                      (DGeometry  *geometry);
GtkWidget*  d_viewport_new_in_position          (DGeometry  *geometry,
                                                 DPos       *pos);

void        d_viewport_set_pos                  (DViewport  *self,
                                                 DPos       *pos);
void        d_viewport_set_axes                 (DViewport  *self,
                                                 DAxes      *axes);
void        d_viewport_set_view_angles          (gdouble    x,
                                                 gdouble    y,
                                                 gdouble    z);
void        d_viewport_increment_view_angles    (gdouble    x,
                                                 gdouble    y,
                                                 gdouble    z);
void        d_viewport_set_far_clip             (gdouble    far_clip);
void        d_viewport_set_near_clip            (gdouble    near_clip);

#endif   /* ----- #ifndef DVIEWER_VIEWPORT_INC  ----- */


