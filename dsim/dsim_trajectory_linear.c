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
 * dsim_trajectory_linear.c :
 */

#include "dsim_trajectory.h"

/* Forward declarations */
static void
d_linear_trajectory_interface_init      (DITrajectoryInterface  *iface);

static void
d_linear_trajectory_class_init          (DLinearTrajectoryClass *klass);

static void
d_linear_trajectory_init                (DLinearTrajectory      *self);

static void
d_linear_trajectory_dispose             (GObject                *obj);

static void
d_linear_trajectory_finalize            (GObject                *obj);

static DVector*
d_linear_trajectory_get_destination     (DITrajectory           *self);

static gboolean
d_linear_trajectory_has_next            (DITrajectory           *self);

static DVector*
d_linear_trajectory_next                (DITrajectory           *self);

static gdouble
d_linear_trajectory_get_step_time       (DITrajectory           *self);

static gdouble
d_linear_trajectory_calculate_move_time (DVector                *deltaC,
                                         DVector                *speed,
                                         gdouble                acceleration_time);

/* Implementation Internals */
G_DEFINE_TYPE_WITH_CODE (DLinearTrajectory,
                         d_linear_trajectory,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (D_TYPE_ITRAJECTORY,
                                                d_linear_trajectory_interface_init));

static void
d_linear_trajectory_interface_init (DITrajectoryInterface   *iface)
{
    iface->get_destination = d_linear_trajectory_get_destination;
    iface->has_next = d_linear_trajectory_has_next;
    iface->next = d_linear_trajectory_next;
    iface->get_step_time = d_linear_trajectory_get_step_time;
}

static void
d_linear_trajectory_class_init (DLinearTrajectoryClass  *klass)
{
    GObjectClass *goc = G_OBJECT_CLASS(klass);
    goc->dispose = d_linear_trajectory_dispose;
    goc->finalize = d_linear_trajectory_finalize;
}

static void
d_linear_trajectory_init (DLinearTrajectory *self)
{
    self->current_pos = NULL;
    self->move_destination = NULL;
    self->start_speed = NULL;
    self->end_speed = NULL;
    self->axes_speed = NULL;
    self->control_point = NULL;
    self->acceleration_time = 0.0;
    self->step_time = 0.0;
    self->move_time = 0.0;
    self->time = 0.0;
}

static void
d_linear_trajectory_dispose (GObject    *obj)
{
    g_warning("d_linear_trajectory_dispose is a stub");

    DLinearTrajectory *self = D_LINEAR_TRAJECTORY(obj);
    if (self->current_pos) {
        g_object_unref(self->current_pos);
        self->current_pos = NULL;
    }
    if (self->move_destination) {
        g_object_unref(self->move_destination);
        self->move_destination = NULL;
    }
    if (self->start_speed) {
        g_object_unref(self->start_speed);
        self->start_speed = NULL;
    }
    if (self->end_speed) {
        g_object_unref(self->end_speed);
        self->end_speed = NULL;
    }
    if (self->control_point) {
        g_object_unref(self->control_point);
        self->control_point = NULL;
    }

    /* Chain up */
    G_OBJECT_CLASS(d_linear_trajectory_parent_class)->dispose(obj);
}

static void
d_linear_trajectory_finalize (GObject   *obj)
{
    /* Chain up */
    G_OBJECT_CLASS(d_linear_trajectory_parent_class)->finalize(obj);
}

static gboolean
d_linear_trajectory_has_next (DITrajectory  *self)
{
    g_return_val_if_fail(D_IS_LINEAR_TRAJECTORY(self), FALSE);

    DLinearTrajectory *linear = D_LINEAR_TRAJECTORY(self);

    if (linear->time < linear->move_time - linear->acceleration_time) {
        return TRUE;
    }
    return FALSE;
}

static DVector*
d_linear_trajectory_get_destination (DITrajectory    *self)
{
    g_return_val_if_fail(D_IS_LINEAR_TRAJECTORY(self), NULL);

    DLinearTrajectory *linear = D_LINEAR_TRAJECTORY(self);

    return linear->move_destination;
}

static gdouble
d_linear_trajectory_get_step_time (DITrajectory     *self)
{
    g_return_val_if_fail(D_IS_LINEAR_TRAJECTORY(self), 0.0);

    DLinearTrajectory *linear = D_LINEAR_TRAJECTORY(self);

    return linear->step_time;
}

static DVector*
d_linear_trajectory_next (DITrajectory  *self)
{
    g_warning("d_linear_trajectory_next is a stub!!!");

    g_return_val_if_fail(D_IS_LINEAR_TRAJECTORY(self), NULL);

    DLinearTrajectory *linear = D_LINEAR_TRAJECTORY(self);

    linear->time += linear->step_time;
    d_trajectory_interpolate_lspb(linear->current_pos,
                                  linear->start_speed,
                                  linear->end_speed,
                                  linear->control_point,
                                  linear->acceleration_time,
                                  linear->time);

    return linear->current_pos;
}

/* Public API */
DLinearTrajectory*
d_linear_trajectory_new (DVector    *currentPosition,
                         DVector    *currentDestination,
                         DVector    *nextDestination,
                         DVector    *aSpeed,
                         DVector    *cSpeed)
{
    return d_linear_trajectory_new_full (currentPosition,
                                         currentDestination,
                                         nextDestination,
                                         aSpeed,
                                         cSpeed,
                                         D_IT_DEFAULT_ACC_TIME,
                                         D_IT_DEFAULT_STEP_TIME );
}

DLinearTrajectory*
d_linear_trajectory_new_full (DVector   *currentPosition,
                              DVector   *currentDestination,
                              DVector   *nextDestination,
                              DVector   *aSpeed,
                              DVector   *cSpeed,
                              gdouble   accTime,
                              gdouble   stepTime)
{
    DLinearTrajectory* dlt;
    dlt = g_object_new (D_TYPE_LINEAR_TRAJECTORY, NULL);
    g_warning("d_linear_trajectory_new_full is a stub!");
    return dlt;
}

