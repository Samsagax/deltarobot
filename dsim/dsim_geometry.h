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
 * dsim_geometry.h :
 */


#ifndef  DSIM_GEOMETRY_INC
#define  DSIM_GEOMETRY_INC

#include <glib-object.h>

/* Type macros */
#define D_TYPE_GEOMETRY             (d_geometry_get_type ())
#define D_GEOMETRY(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_GEOMETRY, DGeometry))
#define D_IS_GEOMETRY(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_GEOMETRY))
#define D_GEOMETRY_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_GEOMETRY, DGeometryClass))
#define D_IS_GEOMETRY_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_GEOMETRY))
#define D_GEOMETRY_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_GEOMETRY, DGeometryClass))

/* Instance Structure of DGeometry */
typedef struct _DGeometry DGeometry;
struct _DGeometry {
    GObject         parent_instance;

    gdouble         a;
    gdouble         b;
    gdouble         h;
    gdouble         r;
};

/* Class Structure of DGeometry */
typedef struct _DGeometryClass DGeometryClass;
struct _DGeometryClass {
    GObjectClass    parent_class;
};

/* Returns GType associated with this object type */
GType       d_geometry_get_type     (void);

/* Create new instance */
DGeometry*  d_geometry_new          ( gdouble a,
                                      gdouble b,
                                      gdouble h,
                                      gdouble r );

#endif   /* ----- #ifndef DSIM_GEOMETRY_INC  ----- */
