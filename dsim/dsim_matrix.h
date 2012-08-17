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
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <dsim/dsim_vector.h>

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

    /* private */
    gsl_matrix      *matrix;
};

/* Class Structure of DMatrix */
typedef struct _DMatrixClass DMatrixClass;
struct _DMatrixClass {
    GObjectClass    parent_class;

    /* Public virtual Methods */
    DMatrix*    (*clone)            (DMatrix *src);

    /* Private virtual methods */
    void        (*set_gsl_matrix)   (DMatrix *self, gsl_matrix *matrix);
};

/* Returns GType associated with this object type */
GType       d_matrix_get_type       (void);

DMatrix*    d_matrix_new            (guint      size1,
                                     guint      size2);

DMatrix*    d_matrix_new_with_gsl   (gsl_matrix *matrix);

DMatrix*    d_matrix_clone          (DMatrix    *src);

gdouble     d_matrix_get            (DMatrix    *self,
                                     guint      i,
                                     guint      j);

void        d_matrix_set            (DMatrix    *self,
                                     guint      i,
                                     guint      j,
                                     gdouble    x);

gint        d_matrix_length         (DMatrix    *self,
                                     gint       dim);

DMatrix*    d_matrix_sub            (DMatrix    *self,
                                     DMatrix    *b);

DMatrix*    d_matrix_add            (DMatrix    *self,
                                     DMatrix    *b);

DMatrix*    d_matrix_mul            (DMatrix    *a,
                                     DMatrix    *b);

DVector*    d_matrix_vector_mul     (DMatrix    *self,
                                     DVector    *v);

DMatrix*    d_matrix_scalar_mul     (DMatrix    *self,
                                     gdouble    a);

gdouble     d_matrix_determinant    (DMatrix    *self);

DMatrix*    d_matrix_inverse        (DMatrix    *self);

#endif   /* ----- #ifndef DSIM_MATRIX_INC  ----- */
