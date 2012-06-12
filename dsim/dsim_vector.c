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
 * dsim_vector.c :
 */

#include "dsim_vector.h"

/* Forward declarations */
static void     d_vector_class_init     (DVectorClass   *klass);

static void     d_vector_init           (DVector        *self);

static void     d_vector_dispose        (GObject        *obj);

static void     d_vector_finalize       (GObject        *obj);

static void     d_vector_set_gsl_vector (DVector        *self,
                                         gsl_vector     *vector);

/* GType register */
G_DEFINE_TYPE (DVector, d_vector, G_TYPE_OBJECT);

static void
d_vector_class_init(DVectorClass* klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_vector_dispose;
    gobject_class->finalize = d_vector_finalize;
}

static void
d_vector_init (DVector *self)
{
    self->vector = NULL;
}

static void
d_vector_dispose (GObject *gobject)
{
    DVector *self = D_VECTOR(gobject);
    if (self->vector) {
        gsl_vector_free(self->vector);
        self->vector = NULL;
    }
    /* Chain up */
    G_OBJECT_CLASS(d_vector_parent_class)->dispose(gobject);
}

static void
d_vector_finalize (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_vector_parent_class)->finalize(gobject);
}

static void
d_vector_set_gsl_vector (DVector    *self,
                         gsl_vector *vector)
{
    if (self->vector) {
        gsl_vector_free(self->vector);
    }
    self->vector = vector;
}

/* Public API */

DVector*
d_vector_new (size_t lenght)
{
    DVector *v;
    v = g_object_new(D_TYPE_VECTOR, NULL);
    gsl_vector *vector = gsl_vector_calloc(lenght);
    d_vector_set_gsl_vector(v, vector);
    return v;
}


DVector*
d_vector_memcpy (DVector    *dest,
                 DVector    *src)
{
    gsl_vector_memcpy(dest->vector, src->vector);
    return dest;
}

gdouble
d_vector_get (DVector   *self,
              size_t    i)
{
    g_return_val_if_fail(D_IS_VECTOR(self), 0);
    g_return_val_if_fail(i < self->vector->size, 0);
    return gsl_vector_get(self->vector, i);
}

void
d_vector_set (DVector   *self,
              size_t    i,
              gdouble   x)
{
    g_return_if_fail(D_IS_VECTOR(self));
    g_return_if_fail(i < self->vector->size);
    gsl_vector_set(self->vector, i, x);
}

DVector*
d_vector_sub (DVector   *a,
              DVector   *b)
{
    g_return_val_if_fail(D_IS_VECTOR(a), NULL);
    g_return_val_if_fail(D_IS_VECTOR(b), NULL);
    g_return_val_if_fail(a->vector->size == a->vector->size, NULL);

    gsl_vector_sub(a->vector, b->vector);

    return a;
}

DVector*
d_vector_add (DVector   *a,
              DVector   *b)
{
    g_return_val_if_fail(D_IS_VECTOR(a), NULL);
    g_return_val_if_fail(D_IS_VECTOR(b), NULL);
    g_return_val_if_fail(a->vector->size == a->vector->size, NULL);

    gsl_vector_sub(a->vector, b->vector);

    return a;
}

gdouble
d_vector_mul (DVector   *a,
              DVector   *b)
{
    gdouble r;
    gsl_blas_ddot(a->vector, b->vector, &r);
    return r;
}

DVector*
d_vector_scalar_mul (DVector    *self,
                     gdouble    a)
{
    gsl_vector_scale(self->vector, a);
    return self;
}

gdouble
d_vector_norm (DVector    *self)
{
    return gsl_blas_dnrm2(self->vector);
}

void
d_vector_normalize (DVector   *self)
{
    gsl_vector_scale(self->vector, gsl_blas_dnrm2(self->vector));
}
