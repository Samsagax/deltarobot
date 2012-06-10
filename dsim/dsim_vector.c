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
#include <math.h>

/* Forward declarations */

/* GType register */
G_DEFINE_TYPE (DVector3, d_vector3, G_TYPE_OBJECT);

DVector3*
d_vector3_new (void)
{
    return g_object_new(D_TYPE_VECTOR3, NULL);
}

DVector3*
d_vector3_new_full (gdouble v1,
                    gdouble v2,
                    gdouble v3)
{
    DVector3 *v = d_vector3_new();
    d_vector3_set(v, 0, v1);
    d_vector3_set(v, 1, v2);
    d_vector3_set(v, 2, v3);
    return v;
}

DVector3*
d_vector3_deep_copy_new (DVector3   *source)
{

}

/* Dispose and finalize functions */
static void
d_vector3_dispose (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_vector3_parent_class)->dispose(gobject);
}

static void
d_vector3_finalize (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_vector3_parent_class)->finalize(gobject);
}

/* Initializer functions */
static void
d_vector3_class_init(DVector3Class* klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_vector3_dispose;
    gobject_class->finalize = d_vector3_finalize;
}

static void
d_vector3_init (DVector3 *self)
{
    self->length = 3;
    for(int i = 0; i < self->length; i++) {
        self->data[i] = 0.0;
    }
}

DVector3*
d_vector3_deep_copy (DVector3   *source,
                     DVector3   *dest)
{
    for (int i = 0; i < source->length; i++) {
        d_vector3_set(dest, i, d_vector3_get(source, i));
    }
    return dest;
}

void
d_vector3_to_string (DVector3  *self,
                          GString   *string)
{
    g_string_printf( string,
                     "[ %f ; %f ; %f ]",
                     self->data[0],
                     self->data[1],
                     self->data[2] );
}

gdouble
d_vector3_get (DVector3    *self,
                    gint        index)
{
    g_return_val_if_fail(D_IS_VECTOR3(self), 0);
    g_return_val_if_fail(index < self->length, 0);
    return self->data[index];
}

void
d_vector3_set (DVector3    *self,
                    gint        index,
                    gdouble     value)
{
    g_return_if_fail(D_IS_VECTOR3(self));
    g_return_if_fail(index < 3);
    self->data[index] = value;
}

DVector3*
d_vector3_substract (DVector3  *a,
                     DVector3  *b)
{
    g_return_val_if_fail(D_IS_VECTOR3(a), NULL);
    g_return_val_if_fail(D_IS_VECTOR3(b), NULL);
    for(int i = 0; i < a->length; i++) {
        a->data[i] = a->data[i] - b->data[i];
    }
    return a;
}

DVector3*
d_vector3_add (DVector3     *a,
               DVector3     *b)
{
    g_return_val_if_fail(D_IS_VECTOR3(a), NULL);
    g_return_val_if_fail(D_IS_VECTOR3(b), NULL);
    for(int i = 0; i < a->length; i++) {
        a->data[i] = a->data[i] + b->data[i];
    }
    return a;
}

gdouble
d_vector3_dot_product (DVector3 *a,
                       DVector3 *b)
{
    gdouble r = 0;
    for (int i = 0; i < a->length; i++) {
        r += a->data[i] * b->data[i];
    }
    return r;
}

DVector3*
d_vector3_cross_product (DVector3   *a,
                         DVector3   *b)
{
    DVector3 *r = d_vector3_new();
    d_vector3_set(r, 0,
                  d_vector3_get(a, 1) * d_vector3_get(b, 2) +
                  d_vector3_get(a, 2) * d_vector3_get(b, 1));
    d_vector3_set(r, 1,
                  d_vector3_get(a, 0) * d_vector3_get(b, 2) -
                  d_vector3_get(a, 2) * d_vector3_get(b, 0));
    d_vector3_set(r, 2,
                  d_vector3_get(a, 0) * d_vector3_get(b, 1) +
                  d_vector3_get(a, 1) * d_vector3_get(b, 0));
    return r;
}

DVector3*
d_vector3_scalar_mult (DVector3 *self,
                       gdouble  a)
{
    for (int i = 0; i < self->length; i++) {
        d_vector3_set(self, i, d_vector3_get(self, i));
    }
}

gdouble
d_vector3_norm (DVector3    *self)
{
    gdouble norm = 0;
    for(int i = 0; i < self->length; i++) {
        norm += pow(d_vector3_get(self,i), 2.0);
    }
    norm = sqrt(norm);
    return norm;
}

void
d_vector3_normalize (DVector3   *self)
{
    gdouble norm = d_vector3_norm(self);
    d_vector3_scalar_mult(self, 1.0 / norm);
}
