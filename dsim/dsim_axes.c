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

#include "dsim_axes.h"

/* ################ Simple DAxes ###################### */

/* Forward declarations */
static void     d_axes_class_init   (DAxesClass     *klass);

static void     d_axes_init         (DAxes          *self);

static void     d_axes_dispose      (GObject        *obj);

static void     d_axes_finalize     (GObject        *obj);

static DVector* d_axes_real_clone   (DVector        *src);

/* Register Type */
G_DEFINE_TYPE (DAxes, d_axes, D_TYPE_VECTOR);

static void
d_axes_dispose (GObject *gobject)
{
    G_OBJECT_CLASS(d_axes_parent_class)->dispose(gobject);
}

static void
d_axes_finalize (GObject *gobject)
{
    G_OBJECT_CLASS (d_axes_parent_class)->finalize (gobject);
}

/* Init functions */
static void
d_axes_init (DAxes  *self)
{
    D_VECTOR(self)->vector = gsl_vector_calloc(3);
}

static void
d_axes_class_init (DAxesClass   *klass)
{
    /* Stub */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_axes_dispose;
    gobject_class->finalize = d_axes_finalize;

    DVectorClass *vector_class = D_VECTOR_CLASS(klass);
    vector_class->clone = d_axes_real_clone;
}

static DVector*
d_axes_real_clone (DVector  *src)
{
    DVector *v = d_axes_new();
    gsl_vector_memcpy(v->vector, src->vector);
    return v;
}

/* Public API */
DVector*
d_axes_new ()
{
    DAxes *axes;
    axes = g_object_new(D_TYPE_AXES, NULL);
    return D_VECTOR(axes);
}

DVector*
d_axes_new_full (gdouble ax1,
                 gdouble ax2,
                 gdouble ax3)
{
    DAxes* a = D_AXES(d_axes_new());
    d_vector_set(D_VECTOR(a), 0, ax1);
    d_vector_set(D_VECTOR(a), 1, ax2);
    d_vector_set(D_VECTOR(a), 2, ax3);
    return D_VECTOR(a);
}

/* ################ Extended DExtAxes ###################### */

/* Forward declarations */
static void d_ext_axes_class_init   (DExtAxesClass  *klass);

static void d_ext_axes_init         (DExtAxes       *self);

static void d_ext_axes_dispose      (GObject        *obj);

static void d_ext_axes_finalize     (GObject        *obj);

/* Register Type */
G_DEFINE_TYPE (DExtAxes, d_ext_axes, G_TYPE_OBJECT);

static void
d_ext_axes_dispose (GObject *gobject)
{
    DExtAxes *self = D_EXTAXES(gobject);
    /* Unref all */
    if (self->axes) {
        gsl_matrix_free(self->axes);
        self->axes = NULL;
    }
    /* Chain up */
    G_OBJECT_CLASS(d_ext_axes_parent_class)->dispose(gobject);
}

static void
d_ext_axes_finalize (GObject *gobject)
{
    G_OBJECT_CLASS (d_ext_axes_parent_class)->finalize (gobject);
}

static void
d_ext_axes_init (DExtAxes   *self)
{
    self->axes = gsl_matrix_calloc(3, 3);
}

static void
d_ext_axes_class_init (DExtAxesClass    *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_ext_axes_dispose;
    gobject_class->finalize = d_ext_axes_finalize;
}

/* Public API */
DExtAxes*
d_ext_axes_new()
{
    return D_EXTAXES(g_object_new(D_TYPE_EXTAXES, NULL));
}

void
d_ext_axes_set (DExtAxes   *self,
                size_t     i,
                size_t     j,
                gdouble    x)
{
    gsl_matrix_set(self->axes, i, j, x);
}

gdouble
d_ext_axes_get (DExtAxes    *self,
                size_t      i,
                size_t      j)
{
    return gsl_matrix_get(self->axes, i, j);
}
