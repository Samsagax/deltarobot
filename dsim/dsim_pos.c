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

#include "dsim_pos.h"

/* Forward declarations */
static void     d_pos_class_init    (DPosClass  *klass);

static void     d_pos_init          (DPos       *self);

static void     d_pos_dispose       (GObject    *obj);

static void     d_pos_finalize      (GObject    *obj);

static DVector* d_pos_real_clone    (DVector    *src);

/* GType register */
G_DEFINE_TYPE (DPos, d_pos, D_TYPE_VECTOR);

static void
d_pos_init (DPos* self)
{
    D_VECTOR(self)->vector = gsl_vector_calloc(3);
}

static void
d_pos_class_init (DPosClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_pos_dispose;
    gobject_class->finalize = d_pos_finalize;

    DVectorClass *vector_class = D_VECTOR_CLASS(klass);
    vector_class->clone = d_pos_real_clone;
}

static void
d_pos_dispose (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_pos_parent_class)->dispose(gobject);
}

static void
d_pos_finalize (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_pos_parent_class)->finalize(gobject);
}

static DVector*
d_pos_real_clone (DVector   *src)
{
    DVector *v = d_pos_new();
    gsl_vector_memcpy(v->vector, src->vector);
    return v;
}

/* Public API */
DVector*
d_pos_new (void)
{
    DPos* pos;
    pos = g_object_new(D_TYPE_POS, NULL);
    return D_VECTOR(pos);
}

DVector*
d_pos_new_full ( gdouble x,
                 gdouble y,
                 gdouble z )
{
    DPos* a = d_pos_new();
    d_vector_set(D_VECTOR(a), 0, x);
    d_vector_set(D_VECTOR(a), 1, y);
    d_vector_set(D_VECTOR(a), 2, z);
    return D_VECTOR(a);
}

