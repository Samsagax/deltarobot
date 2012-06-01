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
 * dsim_vector.h :
 */

#ifndef  DSIM_VECTOR_INC
#define  DSIM_VECTOR_INC

#include <glib-object.h>

/* Type macros */
#define D_TYPE_VECTOR3             (d_vector3_get_type ())
#define D_VECTOR3(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_VECTOR3, DVector3))
#define D_IS_VECTOR3(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_VECTOR3))
#define D_VECTOR3_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_VECTOR3, DVector3Class))
#define D_IS_VECTOR3_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_VECTOR3))
#define D_VECTOR3_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_VECTOR3, DVector3Class))

/* Instance Structure of DVector3 */
typedef struct _DVector3 DVector3;
struct _DVector3 {
    GObject         parent_instance;

    gint            length;
    gdouble         data[3];
};

/* Class Structure of DVector3 */
typedef struct _DVector3Class DVector3Class;
struct _DVector3Class {
    GObjectClass    parent_class;
};

/* Returns GType associated with this object type */
GType       d_vector3_get_type      (void);

/* Create new instance */
DVector3*   d_vector3_new           (void);

DVector3*   d_vector3_new_full      (gdouble    v1,
                                     gdouble    v2,
                                     gdouble    v3);

/* Methods */
DVector3*   d_vector3_copy          (DVector3   *source);

void        d_vector3_to_string     (DVector3   *self,
                                     GString    *string);

gdouble     d_vector3_get           (DVector3   *self,
                                     gint       index);

void        d_vector3_set           (DVector3   *self,
                                     gint       index,
                                     gdouble    value);

DVector3*   d_vector3_substract     (DVector3   *a,
                                     DVector3   *b);

DVector3*   d_vector3_add           (DVector3   *a,
                                     DVector3   *b);

gdouble     d_vector3_dot_product   (DVector3   *a,
                                     DVector3   *b);

DVector3*   d_vector3_cross_product (DVector3   *a,
                                     DVector3   *b);

#endif   /* ----- #ifndef DSIM_VECTOR_INC  ----- */
