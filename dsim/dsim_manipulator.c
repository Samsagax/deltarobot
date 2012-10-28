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
    self->torque = d_vector_new(3);
    self->axes = d_axes_new();
    self->speed = d_speed_new();
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
        g_object_unref(self->torque);
        self->torque = NULL;
    }
    if (self->axes) {
        g_object_unref(self->axes);
        self->axes = NULL;
    }
    if (self->speed) {
        g_object_unref(self->speed);
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
    return d_vector_get(d_manipulator_get_axes(self), axis);
}

void
d_manipulator_set_axis (DManipulator    *self,
                        gint            axis,
                        gdouble         angle)
{
    d_vector_set(d_manipulator_get_axes(self),
            axis,
            angle);
}

DVector*
d_manipulator_get_axes (DManipulator    *self)
{
    return self->axes;
}

void
d_manipulator_set_axes (DManipulator    *self,
                        DVector         *axes)
{
    if (self->axes) {
        g_object_unref(self->axes);
    }
    self->axes = g_object_ref(axes);
}

DVector*
d_manipulator_get_speed (DManipulator   *self)
{
    return self->speed;
}

void
d_manipulator_set_speed (DManipulator   *self,
                         DVector        *speed)
{
    if (self->speed) {
        g_object_unref(self->speed);
    }
    self->speed = g_object_ref(speed);
}

DVector*
d_manipulator_get_torque (DManipulator  *self)
{
    return self->torque;
}

void
d_manipulator_set_torque (DManipulator  *self,
                          DVector       *torque)
{
    if (self->torque) {
        g_object_unref(self->torque);
    }
    self->torque = g_object_ref(torque);
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

