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
    self->torque = NULL;
    self->force = NULL;
    self->axes = NULL;
    self->speed = NULL;
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
    if (self->force) {
        g_object_unref(self->force);
        self->force = NULL;
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

