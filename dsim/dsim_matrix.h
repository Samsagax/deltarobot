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
 * dsim_matrix.h :
 */

#ifndef  DSIM_MATRIX_INC
#define  DSIM_MATRIX_INC

#include <glib-object.h>
#include <math.h>
#include <dsim/dsim_vector.h>
#include <gsl/gsl_matrix.h>

/* Type macros */
#define D_TYPE_MATRIX             (d_matrix_get_type ())
#define D_MATRIX(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_MATRIX, DMatrix))
#define D_IS_MATRIX(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_MATRIX))
#define D_MATRIX_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_MATRIX, DMatrixClass))
#define D_IS_MATRIX_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_MATRIX))
#define D_MATRIX_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_MATRIX, DMatrixClass))

/* Instance Structure of DMatrix */
typedef struct _DMatrix DMatrix;
struct _DMatrix {
    GObject         parent_instance;

    gsl_matrix      *matrix
};

/* Class Structure of DMatrix */
typedef struct _DMatrixClass DMatrixClass;
struct _DMatrixClass {
    GObjectClass    parent_class;
};

/* Returns GType associated with this object type */
GType       d_matrix_get_type       (void);

/* Create new instance */
DMatrix*    d_matrix_new            (size_t     rows,
                                     size_t     columns);

/* Methods */
DMatrix*    d_matrix_memcpy         (DMatrix    *dest,
                                     DMatrix    *src);

gdouble     d_matrix_get            (DMatrix    *self,
                                     size_t     i,
                                     size_t     j);

void        d_matrix_set            (DMatrix   *self,
                                     size_t     i,
                                     size_t     j,
                                     gdouble    x);

DMatrix*    d_matrix_sub            (DMatrix   *a,
                                     DMatrix   *b);

DMatrix*    d_matrix_add            (DMatrix   *a,
                                     DMatrix   *b);

DMatrix*    d_matrix_mul            (DMatrix   *a,
                                     DMatrix   *b);

DMatrix*    d_matrix_scalar_mul     (DMatrix   *self,
                                     gdouble    a);

DVector*    d_matrix_vector_mul     (DMatrix   *self,
                                     DVector   *v);

#endif   /* ----- #ifndef DSIM_MATRIX_INC  ----- */
