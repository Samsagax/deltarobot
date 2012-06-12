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
 * dsim_matrix.c :
 */

#include "dsim_matrix.h"

/* Forward declarations */
static void     d_matrix_class_init     (DMatrixClass  *klass);

static void     d_matrix_init           (DMatrix       *self);

static void     d_matrix_dispose        (GObject        *obj);

static void     d_matrix_finalize       (GObject        *obj);

static void     d_matrix_set_gsl_matrix (DMatrix        *self,
                                         gsl_matrix     *m);

/* Register Type */
G_DEFINE_TYPE(DMatrix, d_matrix, G_TYPE_OBJECT);

static void
d_matrix_class_init (DMatrixClass *klass)
{
    GObjectClass *gobjclass = G_OBJECT_CLASS(klass);

    gobjclass->dispose = d_matrix_dispose;
    gobjclass->finalize = d_matrix_finalize;
}

static void
d_matrix_init (DMatrix    *self)
{
    self->matrix = NULL;
}

static void
d_matrix_dispose (GObject  *obj)
{
    DMatrix *self = D_MATRIX(obj);
    if (self->matrix) {
        gsl_matrix_free(self->matrix);
        self->matrix = NULL;
    }
    /* Chain Up */
    G_OBJECT_CLASS(d_matrix_parent_class)->dispose(obj);
}

static void
d_matrix_finalize (GObject *obj)
{
    /* Chain Up */
    G_OBJECT_CLASS(d_matrix_parent_class)->finalize(obj);
}

static void
d_matrix_set_gsl_matrix (DMatrix    *self,
                         gsl_matrix *m)
{
    if (self->matrix) {
        gsl_matrix_free(self->matrix);
    }
    self->matrix = m;
}

/* Public API */

DMatrix*
d_matrix_new (size_t    rows,
              size_t    columns)
{
    DMatrix* m;
    m = g_object_new (D_TYPE_MATRIX, NULL);
    d_matrix_set_gsl_matrix(m, gsl_matrix_calloc(rows, columns));
    return m;
}

DMatrix*
d_matrix_memcpy (DMatrix    *dest,
                 DMatrix    *src)
{
    gsl_matrix_memcpy(dest->matrix, src->matrix);
    return dest;
}

gdouble
d_matrix_get (DMatrix   *self,
              size_t    i,
              size_t    j)
{
    return gsl_matrix_get(self->matrix, i, j);
}

void
d_matrix_set (DMatrix   *self,
              size_t    i,
              size_t    j,
              gdouble   x)
{
    gsl_matrix_set(self->matrix, i, j, x);
}

DMatrix*
d_matrix_sub (DMatrix   *a,
              DMatrix   *b)
{
    gsl_matrix_sub(a->matrix, b->matrix);
    return a;
}

DMatrix*
d_matrix_add (DMatrix   *a,
              DMatrix   *b)
{
    gsl_matrix_add(a->matrix, b->matrix);
    return a;
}

DMatrix*
d_matrix_mul (DMatrix   *a,
              DMatrix   *b)
{
    DMatrix *c;
    c = d_matrix_new (a->matrix->size1, b->matrix->size2);
    gsl_blas_dgemm (CblasNoTrans, CblasNoTrans, 1.0, a->matrix, b->matrix, 0.0, c->matrix);
    return c;
}

DMatrix*
d_matrix_scalar_mul (DMatrix    *self,
                     gdouble    a)
{
    gsl_matrix_scale(self->matrix, a);
    return self;
}

DVector*
d_matrix_vector_mul (DMatrix    *self,
                     DVector    *v)
{
    DVector *a;
    a = d_vector_new (self->matrix->size1);
    gsl_blas_dgemv(CblasNoTrans, 1.0, self->matrix, v->vector, 0.0, a->vector);
    return a;
}
