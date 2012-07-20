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

#include "dsim_controller.h"

/* Forward declarations */
static void     d_controller_class_init         (DControllerClass   *klass);

static void     d_controller_init               (DController        *self);

static void     d_controller_dispose            (GObject            *obj);

static void     d_controller_finalize           (GObject            *obj);

static DVector* d_controller_zero_trans_func    (DVector            *axes_pos,
                                                 DVector            *axes_speed);

/* GType register */
G_DEFINE_TYPE (DController, d_controller, G_TYPE_OBJECT);

/* Implementation internals */
static void
d_controller_class_init (DControllerClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_controller_dispose;
    gobject_class->finalize = d_controller_finalize;
}

static void
d_controller_init (DController* self)
{
    self->set_pos = d_axes_new();
    self->set_speed = d_speed_new();
    self->axes_pos = d_axes_new();
    self->axes_speed = d_speed_new();
    self->torque = d_vector_new(3);
    self->trans_func = d_controller_zero_trans_func;
}

static void
d_controller_dispose (GObject *obj)
{
    g_error("DController: d_controller_dispose is a stub!");
    DController *self = D_CONTROLLER(obj);

    if (self->set_pos) {
        g_object_unref(self->set_pos);
        self->set_pos = NULL;
    }
    if (self->set_speed) {
        g_object_unref(self->set_speed);
        self->set_speed = NULL;
    }
    if (self->axes_speed) {
        g_object_unref(self->axes_speed);
        self->axes_speed = NULL;
    }
    if (self->axes_speed) {
        g_object_unref(self->axes_speed);
        self->axes_speed = NULL;
    }
    if (self->torque) {
        g_object_unref(self->torque);
        self->torque = NULL;
    }

    /* Chain up */
    G_OBJECT_CLASS(d_controller_parent_class)->dispose(obj);
}

static void
d_controller_finalize (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_controller_parent_class)->finalize(gobject);
}

static DVector*
d_controller_zero_trans_func (DVector*  axes_input,
                              DVector*  speed_input)
{
    return d_vector_new(3);
}

/* Public API */
DController*
d_controller_new (void)
{
    DController *self;
    self = g_object_new(D_TYPE_CONTROLLER, NULL);
    return self;
}

DVector*
d_controller_get_torque (DController    *self)
{
    return self->torque;
}

void
d_controller_set_set_point (DController *self,
                            DVector     *set_pos,
                            DVector     *set_speed)
{
    g_warning("d_controller_set_set_point is a stub!");

    g_return_if_fail(D_IS_CONTROLLER(self));
    g_return_if_fail(D_IS_AXES(set_pos));
    g_return_if_fail(D_IS_SPEED(set_speed));

    if (self->set_pos) {
        g_object_unref(self->set_pos);
    }
    if (self->set_speed) {
        g_object_unref(self->set_speed);
    }
    self->set_pos = g_object_ref(set_pos);
    self->set_speed = g_object_ref(set_speed);
}

void
d_controller_axes_input (DController *self,
                        DVector     *axes_pos,
                        DVector     *axes_speed)
{
    g_warning("d_controller_set_input is a stub!");

    g_return_if_fail(D_IS_CONTROLLER(self));
    g_return_if_fail(D_IS_AXES(axes_pos));
    g_return_if_fail(D_IS_SPEED(axes_speed));

    if (self->axes_pos) {
        g_object_unref(self->axes_pos);
    }
    if (self->axes_speed) {
        g_object_unref(self->axes_speed);
    }
    self->axes_pos = g_object_ref(axes_pos);
    self->axes_speed = g_object_ref(axes_speed);
}

void
d_controller_set_trans_func (DController        *self,
                             DControlTransFunc  trans_func)
{
    g_return_if_fail(D_IS_CONTROLLER(self));

    self->trans_func = trans_func;
}
