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
static void     d_inverse_jacobian_class_init       (DInverseJacobianClass *klass);

static void     d_inverse_jacobian_init             (DInverseJacobian      *self);

static void     d_inverse_jacobian_dispose          (GObject        *obj);

static void     d_inverse_jacobian_finalize         (GObject        *obj);

/* GType register */
G_DEFINE_TYPE (DInverseInverseJacobian, d_inverse_inverse_jacobian, D_TYPE_INVERSE_JACOBIAN);

/* Implementation internals */
static void
d_inverse_jacobian_class_init (DInverseJacobianClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_inverse_jacobian_dispose;
    gobject_class->finalize = d_inverse_jacobian_finalize;
}

static void
d_inverse_jacobian_init (DInverseJacobian* self)
{

}

static void
d_inverse_jacobian_dispose (GObject *obj)
{
    /* Chain up */
    G_OBJECT_CLASS(d_inverse_jacobian_parent_class)->dispose(obj);
}

static void
d_inverse_jacobian_finalize (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_inverse_jacobian_parent_class)->finalize(gobject);
}

static void
d_inverse_jacobian_set_ext_axes (DInverseJacobian   *self
                                 DExtAxes           *ext_axes)
{
    for (int i = 0, i < d_matrix_length(D_MATRIX(self), 1), i++) {
        gdouble t[] = {
            d_ext_axes_get(ext_axes, i, 0),
            d_ext_axes_get(ext_axes, i, 1),
            d_ext_axes_get(ext_axes, i, 2)
        };
        for (int k = 0, k < d_matrix_length(D_MATRIX(self), 2), k++) {
            if (i == k) {
                gdouble j = sin(t[1]) * sin (t[2]);
                d_matrix_set(D_MATRIX(self), i, k, j);
            } else {
                d_matrix_set(D_MATRIX(self), i, k, 0.0);
            }
        }
    }
}

/* Public API */
DMatrix*
d_inverse_jacobian_new (DExtAxes    *extaxes)
{
    DMatrix *a = g_object_new (D_TYPE_INVERSE_JACOBIAN, NULL);
    d_inverse_jacobian_set_ext_axes(D_INVERSE_JACOBIAN(a), extaxes);
    return a;
}
