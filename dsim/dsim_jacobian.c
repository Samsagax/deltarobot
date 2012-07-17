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
static void     d_jacobian_class_init       (DJacobianClass *klass);

static void     d_jacobian_init             (DJacobian      *self);

static void     d_jacobian_dispose          (GObject        *obj);

static void     d_jacobian_finalize         (GObject        *obj);

static gboolean d_jacobian_real_is_singular (DJacobian      *self);

/* GType register */
G_DEFINE_TYPE (DJacobian, d_jacobian, D_TYPE_MATRIX);

/* Implementation internals */
static void
d_jacobian_class_init (DJacobianClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_jacobian_dispose;
    gobject_class->finalize = d_jacobian_finalize;

    klass->is_singular = d_jacobian_real_is_singular;
}

static void
d_jacobian_init (DJacobian* self)
{
    D_MATRIX(self)->matrix = gsl_matrix_calloc(3, 3);
}

static void
d_jacobian_dispose (GObject *obj)
{
    /* Chain up */
    G_OBJECT_CLASS(d_jacobian_parent_class)->dispose(obj);
}

static void
d_jacobian_finalize (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_jacobian_parent_class)->finalize(gobject);
}

static gboolean
d_jacobian_real_is_singular (DJacobian  *self)
{
    if (d_matrix_determinant(D_MATRIX(self)) < FLT_EPSILON) {
        return TRUE;
    }
    return FALSE;
}

/* Public API */
gboolean
d_jacobian_is_singular (DJacobian   *self)
{
    return D_JACOBIAN_GET_CLASS(self)->is_singular(self);
}
