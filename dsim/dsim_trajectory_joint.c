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
 * dsim_trajectory_joint.c :
 */

#include "dsim_trajectory.h"

/* Forward declarations */
static void
d_joint_trajectory_class_init           (DJointTrajectoryClass  *klass);

static void
d_joint_trajectory_init                 (DJointTrajectory       *self);

static void
d_joint_trajectory_dispose              (GObject                *obj);

static void
d_joint_trajectory_finalize             (GObject                *obj);

/* Implementation internals */
G_DEFINE_TYPE (DJointTrajectory, d_joint_trajectory, D_TYPE_TRAJECTORY);

static void
d_joint_trajectory_class_init (DJointTrajectoryClass    *klass)
{
    GObjectClass *goc = G_OBJECT_CLASS(klass);
    goc->dispose = d_joint_trajectory_dispose;
    goc->finalize = d_joint_trajectory_finalize;
}

static void
d_joint_trajectory_init (DJointTrajectory   *self)
{
}

static void
d_joint_trajectory_dispose (GObject    *obj)
{
    /* Chain up */
    G_OBJECT_CLASS(d_joint_trajectory_parent_class)->dispose(obj);
}

static void
d_joint_trajectory_finalize ( GObject   *obj )
{
    /* Chain up */
    G_OBJECT_CLASS(d_joint_trajectory_parent_class)->finalize(obj);
}

/* Public API */
DJointTrajectory*
d_joint_trajectory_new (DVector *current_axes,
                        DVector *control_point,
                        DVector *move_destination,
                        DVector *max_speed)
{
    return d_joint_trajectory_new_full (current_axes,
                                        control_point,
                                        move_destination,
                                        max_speed,
                                        D_DEFAULT_ACC_TIME,
                                        D_DEFAULT_STEP_TIME);
}

DJointTrajectory*
d_joint_trajectory_new_full (DVector    *current_axes,
                             DVector    *control_point,
                             DVector    *move_destination,
                             DVector    *max_speed,
                             gdouble    acceleration_time,
                             gdouble    step_time)
{
    DJointTrajectory *self;
    DTrajectory *parent;
    self = g_object_new (D_TYPE_JOINT_TRAJECTORY, NULL);
    parent = D_TRAJECTORY(self);

    parent->time = -acceleration_time;
    parent->acceleration_time = acceleration_time;

    parent->current = d_vector_clone(current_axes);
    parent->destination = d_vector_clone(move_destination);

    parent->start_speed = d_vector_clone(control_point);
    d_vector_sub(parent->start_speed, current_axes);
    d_vector_scalar_mul(D_VECTOR(parent->start_speed), 1.0 / acceleration_time);

    DVector *displacement = d_vector_clone(move_destination);
    d_vector_sub(displacement, control_point);

    parent->move_time = d_trajectory_calculate_move_time(displacement,
                                                         max_speed,
                                                         acceleration_time);

    parent->end_speed = d_speed_new_from_displacement(displacement, parent->move_time);

    parent->control_point = d_vector_clone(D_VECTOR(control_point));

    parent->step_time = step_time;

    g_object_unref(displacement);

    return self;
}

