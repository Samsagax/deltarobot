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
 * dsim_manipulator.c :
 */

#include "dsim_manipulator.h"

/* Forward declarations */
static void         d_manipulator_class_init   (DManipulatorClass  *klass);

static void         d_manipulator_init         (DManipulator       *self);

static void         d_manipulator_dispose      (GObject            *obj);

static void         d_manipulator_finalize     (GObject            *obj);

/* GType Register */
G_DEFINE_TYPE(DManipulator, d_manipulator, G_TYPE_OBJECT);

/* DManipulator implementaion */
static void
d_manipulator_init (DManipulator   *self)
{
    self->geometry = NULL;
    self->dynamic_params = NULL;
    self->torque = gsl_vector_calloc(3);
    self->axes = gsl_vector_calloc(3);
    self->speed = gsl_vector_calloc(3);
}

static void
d_manipulator_class_init (DManipulatorClass    *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_manipulator_dispose;
    gobject_class->finalize = d_manipulator_finalize;
}

static void
d_manipulator_dispose (GObject *gobject)
{
    DManipulator *self = D_MANIPULATOR(gobject);

    if (self->geometry) {
        g_object_unref(self->geometry);
        self->geometry = NULL;
    }
    if (self->dynamic_params) {
        g_object_unref(self->dynamic_params);
        self->dynamic_params = NULL;
    }
    if (self->torque) {
        gsl_vector_free(self->torque);
        self->torque = NULL;
    }
    if (self->axes) {
        gsl_vector_free(self->axes);
        self->axes = NULL;
    }
    if (self->speed) {
        gsl_vector_free(self->speed);
        self->speed = NULL;
    }

    /* Chain Up */
    G_OBJECT_CLASS(d_manipulator_parent_class)->dispose(gobject);
}

static void
d_manipulator_finalize (GObject *gobject)
{
    /* Chain Up */
    G_OBJECT_CLASS (d_manipulator_parent_class)->finalize (gobject);
}

static void
d_manipulator_set_dynamic_spec (DManipulator    *self,
                                DDynamicSpec    *dynamic_params)
{
    if (self->dynamic_params) {
        g_object_unref(self->dynamic_params);
    }
    self->dynamic_params = g_object_ref(dynamic_params);
}

static void
d_manipulator_set_geometry (DManipulator    *self,
                            DGeometry       *geometry)
{
    if (self->geometry) {
        g_object_unref(self->geometry);
    }
    self->geometry = g_object_ref(geometry);
}

/* Public API */
DManipulator*
d_manipulator_new (DGeometry    *geometry,
                   DDynamicSpec *dynamic_params)
{
    DManipulator *dm;
    dm = g_object_new(D_TYPE_MANIPULATOR, NULL);

    d_manipulator_set_geometry(dm, geometry);
    d_manipulator_set_dynamic_spec(dm, dynamic_params);

    return dm;
}

gdouble
d_manipulator_get_axis (DManipulator    *self,
                        gint            axis)
{
    return gsl_vector_get(self->axes, axis);
}

gsl_vector*
d_manipulator_get_axes (DManipulator    *self)
{
    g_return_val_if_fail(D_IS_MANIPULATOR(self), 0);

    return self->axes;
}

void
d_manipulator_set_axis (DManipulator    *self,
                        gint            axis,
                        gdouble         angle)
{
    g_return_if_fail(D_IS_MANIPULATOR(self));

    gsl_vector_set(self->axes,
            axis,
            angle);
}

void
d_manipulator_set_axes (DManipulator    *self,
                        gsl_vector      *axes)
{
    g_return_if_fail(D_IS_MANIPULATOR(self));

    gsl_vector_memcpy(self->axes, axes);
}

gsl_vector*
d_manipulator_get_speed (DManipulator  *self)
{
    g_return_val_if_fail(D_IS_MANIPULATOR(self), 0);

    return self->speed;
}

gdouble
d_manipulator_get_axis_speed (DManipulator  *self,
                              gint          axis)
{
    g_return_val_if_fail(D_IS_MANIPULATOR(self), 0);
    g_return_val_if_fail(axis < 3, 0);

    return gsl_vector_get(self->speed, axis);
}

void
d_manipulator_set_axis_speed (DManipulator  *self,
                              gint          axis,
                              gdouble       q_dot)
{
    g_return_if_fail(D_IS_MANIPULATOR(self));
    g_return_if_fail(axis < 3);

    gsl_vector_set(self->speed, axis, q_dot);
}

void
d_manipulator_set_speed (DManipulator   *self,
                         gsl_vector     *speed)
{
    g_return_if_fail(D_IS_MANIPULATOR(self));

    gsl_vector_memcpy(self->speed, speed);
}

gsl_vector*
d_manipulator_get_torque (DManipulator  *self)
{
    return self->torque;
}

void
d_manipulator_set_torque (DManipulator  *self,
                          gsl_vector    *torque)
{
    g_return_if_fail(D_IS_MANIPULATOR(self));

    gsl_vector_memcpy(self->torque, torque);
}

DDynamicSpec*
d_manipulator_get_dynamic_spec (DManipulator    *self)
{
    return self->dynamic_params;
}

DGeometry*
d_manipulator_get_geometry (DManipulator    *self)
{
    return self->geometry;
}

