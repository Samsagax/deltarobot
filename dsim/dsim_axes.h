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

#ifndef  DSIM_AXES_INC
#define  DSIM_AXES_INC

#include <glib-object.h>
#include <dsim/dsim_vector.h>

/* Type macros Simple Axes */
#define D_TYPE_AXES             (d_axes_get_type ())
#define D_AXES(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_AXES, DAxes))
#define D_IS_AXES(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_AXES))
#define D_AXES_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_AXES, DAxesClass))
#define D_IS_AXES_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_AXES))
#define D_AXES_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_AXES, DAxesClass))

/* Instance Structure of DAxes */
typedef struct _DAxes DAxes;
struct _DAxes {
    DVector3        parent_instance;
};

/* Class Structure of DAxes */
typedef struct _DAxesClass DAxesClass;
struct _DAxesClass {
    DVector3Class   parent_class;
};

/* Returns GType associated with this object type */
GType   d_axes_get_type (void);

/* Create new instance */
DVector3*   d_axes_new          (void);
DVector3*   d_axes_new_full     (gdouble ax1, gdouble ax2, gdouble ax3);

/* Type macros Extended ExtAxes */
#define D_TYPE_EXTAXES             (d_ext_axes_get_type ())
#define D_EXTAXES(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_EXTAXES, DExtAxes))
#define D_IS_EXTAXES(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_EXTAXES))
#define D_EXTAXES_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_EXTAXES, DExtAxesClass))
#define D_IS_EXTAXES_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_EXTAXES))
#define D_EXTAXES_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_EXTAXES, DExtAxesClass))

/* Instance Structure of DExtAxes */
typedef struct _DExtAxes DExtAxes;
struct _DExtAxes {
    GObject         parent_instance;
    DAxes           *axes[3];           /* Axes[limb][angle] */
};

/* Class Structure of DExtAxes */
typedef struct _DExtAxesClass DExtAxesClass;
struct _DExtAxesClass {
    GObjectClass    parent_class;
};

/* Returns GType associated with this object type */
GType       d_ext_axes_get_type     (void);

/* Create new instance */
DExtAxes*   d_ext_axes_new          (void);
void        d_ext_axes_set          ( DExtAxes *self,
                                      gint      i,
                                      gint      j,
                                      gdouble   value);
gdouble     d_ext_axes_get          ( DExtAxes *self,
                                      gint      i,
                                      gint      j );

#endif   /* ----- #ifndef DSIM_EXTAXES_INC  ----- */
