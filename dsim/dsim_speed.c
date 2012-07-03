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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURSPEEDE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PROJECTNAME. If not, see <http://www.gnu.org/licenses/>.
 */

#include "dsim_speed.h"

/* GType register */
G_DEFINE_TYPE (DSpeed, d_speed, D_TYPE_VECTOR);

/* Dipose and finalize functions */
static void
d_speed_dispose (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_speed_parent_class)->dispose(gobject);
}

static void
d_speed_finalize (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_speed_parent_class)->finalize(gobject);
}

/* Initialization functions */
static void
d_speed_init (DSpeed* self)
{
    D_VECTOR(self)->vector = gsl_vector_calloc(3);
}

static void
d_speed_class_init (DSpeedClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_speed_dispose;
    gobject_class->finalize = d_speed_finalize;
}

/* Public API */
DVector*
d_speed_new (void)
{
    DVector *a = g_object_new (D_TYPE_SPEED, NULL);
    return a;
}

DVector*
d_speed_new_full (gdouble   s1,
                  gdouble   s2,
                  gdouble   s3)
{
    DVector *a = d_speed_new();
    d_vector_set(a, 0, s1);
    d_vector_set(a, 1, s2);
    d_vector_set(a, 2, s3);
    return a;
}

DVector*
d_speed_new_from_displacement (DVector  *displacement,
                               gdouble  time)
{
    g_return_val_if_fail(D_IS_VECTOR(displacement), NULL);
    DVector *s = d_speed_new();
    for (int i = 0; i < d_vector_length(s); i++) {
        d_vector_set(s, i, d_vector_get(displacement, i) / time);
    }
    return s;
}
