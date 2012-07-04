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
d_linear_trajectory_class_init          (DLinearTrajectoryClass *klass);

static void
d_linear_trajectory_init                (DLinearTrajectory      *self);

static void
d_linear_trajectory_dispose             (GObject                *obj);

static void
d_linear_trajectory_finalize            (GObject                *obj);

/* Implementation Internals */
G_DEFINE_TYPE (DLinearTrajectory, d_linear_trajectory, D_TYPE_TRAJECTORY);

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
}

static void
d_linear_trajectory_dispose (GObject    *obj)
{
    /* Chain up */
    G_OBJECT_CLASS(d_linear_trajectory_parent_class)->dispose(obj);
}

static void
d_linear_trajectory_finalize (GObject   *obj)
{
    /* Chain up */
    G_OBJECT_CLASS(d_linear_trajectory_parent_class)->finalize(obj);
}

/* Public API */
DLinearTrajectory*
d_linear_trajectory_new (DVector    *current_pos,
                         DVector    *control_point,
                         DVector    *move_destination,
                         DVector    *speed)
{
    return d_linear_trajectory_new_full (current_pos,
                                         control_point,
                                         move_destination,
                                         speed,
                                         D_DEFAULT_ACC_TIME,
                                         D_DEFAULT_STEP_TIME );
}

DLinearTrajectory*
d_linear_trajectory_new_full (DVector   *current_pos,
                              DVector   *control_point,
                              DVector   *move_destination,
                              DVector   *speed,
                              gdouble   acceleration_time,
                              gdouble   step_time)
{
    g_warning("d_linear_trajectory_new_full is a stub!");

    DLinearTrajectory* self;
    DTrajectory *parent;
    self = g_object_new (D_TYPE_LINEAR_TRAJECTORY, NULL);
    parent = D_TRAJECTORY(self);

    parent->time = -acceleration_time;
    parent->acceleration_time = acceleration_time;

    parent->current = d_vector_clone(current_pos);
    parent->destination = d_vector_clone(move_destination);

    parent->start_speed = d_vector_clone(control_point);
    d_vector_sub(parent->start_speed, current_pos);
    d_vector_scalar_mul(parent->start_speed, 1.0 / acceleration_time);

    DVector *displacement = d_vector_clone(move_destination);
    d_vector_sub(displacement, control_point);

    parent->move_time = d_trajectory_calculate_move_time(displacement,
                                                         speed,
                                                         acceleration_time);

    parent->end_speed = d_speed_new_from_displacement(displacement, parent->move_time);

    parent->control_point = d_vector_clone(control_point);

    parent->step_time = step_time;

    g_object_unref(displacement);

    return self;
}

