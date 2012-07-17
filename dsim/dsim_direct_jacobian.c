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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURJACOBIANE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PROJECTNAME. If not, see <http://www.gnu.org/licenses/>.
 */

#include "dsim_jacobian.h"

/* Forward declarations */
static void     d_direct_jacobian_class_init    (DDirectJacobianClass   *klass);

static void     d_direct_jacobian_init          (DDirectJacobian        *self);

static void     d_direct_jacobian_dispose       (GObject                *obj);

static void     d_direct_jacobian_finalize      (GObject                *obj);

/* GType register */
G_DEFINE_TYPE (DDirectJacobian, d_direct_jacobian, D_TYPE_JACOBIAN);

/* Implementation internals */
static void
d_direct_jacobian_class_init (DDirectJacobianClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_direct_jacobian_dispose;
    gobject_class->finalize = d_direct_jacobian_finalize;
}

static void
d_direct_jacobian_init (DDirectJacobian* self)
{
}

static void
d_direct_jacobian_dispose (GObject *obj)
{
    /* Chain up */
    G_OBJECT_CLASS(d_direct_jacobian_parent_class)->dispose(obj);
}

static void
d_direct_jacobian_finalize (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_direct_jacobian_parent_class)->finalize(gobject);
}

static void
d_direct_jacobian_set_ext_axes (DDirectJacobian  *self,
                                DExtAxes         *ext_axes)
{
    for (int i = 0; i < d_matrix_length(D_MATRIX(self), 1); i++) {
        gdouble phi = i * G_PI * 120.0 / 180.0;
        gdouble t[] = {
            d_ext_axes_get(ext_axes, i, 0),
            d_ext_axes_get(ext_axes, i, 1),
            d_ext_axes_get(ext_axes, i, 2)
        };
        gdouble j[] = {
            cos(t[0] + t[1]) * sin(t[2]) * cos(phi) - cos(t[2]) * sin(phi),
            cos(t[0] + t[1]) * sin(t[2]) * sin(phi) + cos(t[2]) * cos(phi),
            sin(t[0] + t[1]) * sin(t[2])
        };
        for (int k = 0; k < d_matrix_length(D_MATRIX(self), 2); k++) {
            d_matrix_set(D_MATRIX(self), i, k, j[k]);
        }
    }
}

/* Public API */
DMatrix*
d_direct_jacobian_new (DExtAxes *extaxes)
{
    DMatrix *a = g_object_new (D_TYPE_DIRECT_JACOBIAN, NULL);
    d_direct_jacobian_set_ext_axes (D_DIRECT_JACOBIAN(a), extaxes);
    return a;
}

