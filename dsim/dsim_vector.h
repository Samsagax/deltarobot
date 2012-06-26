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
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

/* Type macros */
#define D_TYPE_VECTOR             (d_vector_get_type ())
#define D_VECTOR(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_VECTOR, DVector))
#define D_IS_VECTOR(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_VECTOR))
#define D_VECTOR_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_VECTOR, DVectorClass))
#define D_IS_VECTOR_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_VECTOR))
#define D_VECTOR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_VECTOR, DVectorClass))

/* Instance Structure of DVector */
typedef struct _DVector DVector;
struct _DVector {
    GObject         parent_instance;

    gsl_vector      *vector;
};

/* Class Structure of DVector */
typedef struct _DVectorClass DVectorClass;
struct _DVectorClass {
    GObjectClass    parent_class;
};

/* Returns GType associated with this object type */
GType       d_vector_get_type      (void);

/* Create new instance */
DVector*    d_vector_new            (size_t     size);

DVector*    d_vector_new_gsl        (gsl_vector *vector);

/* Methods */
DVector*    d_vector_clone          (DVector    *src);

gdouble     d_vector_get            (DVector    *self,
                                     size_t     i);

void        d_vector_set            (DVector    *self,
                                     size_t     i,
                                     gdouble    x);

DVector*    d_vector_sub            (DVector    *a,
                                     DVector    *b);

DVector*    d_vector_add            (DVector    *a,
                                     DVector    *b);

gdouble     d_vector_mul            (DVector    *a,
                                     DVector    *b);

DVector*    d_vector_scalar_mul     (DVector    *self,
                                     gdouble    a);

gdouble     d_vector_norm           (DVector    *self);

void        d_vector_normalize      (DVector    *self);

#endif   /* ----- #ifndef DSIM_VECTOR_INC  ----- */
