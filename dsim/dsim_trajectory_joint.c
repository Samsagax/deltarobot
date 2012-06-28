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

static void
d_joint_trajectory_interface_init       (DITrajectoryInterface  *iface);

static DVector*
d_joint_trajectory_get_destination      (DITrajectory           *self);

static gboolean
d_joint_trajectory_has_next             (DITrajectory           *self);

static DVector*
d_joint_trajectory_next                 (DITrajectory           *self);

static gdouble
d_joint_trajectory_get_step_time        (DITrajectory           *self);

static gdouble
d_joint_trajectory_calculate_move_time  (DVector                *deltaC,
                                         DVector                *speed,
                                         gdouble                accTime);

/* Implementation internals */
G_DEFINE_TYPE_WITH_CODE (DJointTrajectory,
                         d_joint_trajectory,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (D_TYPE_ITRAJECTORY,
                                                d_joint_trajectory_interface_init));

static void
d_joint_trajectory_class_init (DJointTrajectoryClass    *klass)
{
    GObjectClass *goc = G_OBJECT_CLASS(klass);
    goc->dispose = d_joint_trajectory_dispose;
    goc->finalize = d_joint_trajectory_finalize;
}

static void
d_joint_trajectory_interface_init (DITrajectoryInterface    *iface)
{
    iface->get_destination = d_joint_trajectory_get_destination;
    iface->has_next = d_joint_trajectory_has_next;
    iface->next = d_joint_trajectory_next;
    iface->get_step_time = d_joint_trajectory_get_step_time;
}

static void
d_joint_trajectory_init (DJointTrajectory   *self)
{
    self->current_axes = NULL;
    self->move_destination = NULL;

    self->start_speed = NULL;
    self->end_speed = NULL;
    self->control_point = NULL;

    self->acceleration_time = 0.0;
    self->step_time = 0.0;
    self->move_time = 0.0;
    self->step_time = 0.0;
}

static void
d_joint_trajectory_dispose (GObject    *obj)
{
    DJointTrajectory *self = D_JOINT_TRAJECTORY(obj);

    if (self->current_axes) {
        g_object_unref(self->current_axes);
        self->current_axes = NULL;
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
    G_OBJECT_CLASS(d_joint_trajectory_parent_class)->dispose(obj);
}

static void
d_joint_trajectory_finalize ( GObject   *obj )
{
    /* Chain up */
    G_OBJECT_CLASS(d_joint_trajectory_parent_class)->finalize(obj);
}

/* Methods */
static gboolean
d_joint_trajectory_has_next (DITrajectory   *self)
{
    g_return_val_if_fail(D_IS_JOINT_TRAJECTORY(self), FALSE);

    DJointTrajectory *joint = D_JOINT_TRAJECTORY(self);

    if (joint->time < joint->move_time - joint->acceleration_time) {
        return TRUE;
    }
    return FALSE;
}

static DVector*
d_joint_trajectory_get_destination (DITrajectory    *self)
{
    g_return_val_if_fail(D_IS_JOINT_TRAJECTORY(self), NULL);

    DJointTrajectory *joint = D_JOINT_TRAJECTORY(self);

    return joint->move_destination;
}

static DVector*
d_joint_trajectory_next (DITrajectory   *self)
{
    g_return_val_if_fail(D_IS_JOINT_TRAJECTORY(self), NULL);

    DJointTrajectory *joint = D_JOINT_TRAJECTORY(self);

    joint->time += joint->step_time;
    d_trajectory_interpolate_lspb(joint->current_axes,
                                  joint->start_speed,
                                  joint->end_speed,
                                  joint->control_point,
                                  joint->acceleration_time,
                                  joint->time);

    return joint->current_axes;
}

static gdouble
d_joint_trajectory_get_step_time (DITrajectory  *self)
{
    g_return_val_if_fail(D_IS_JOINT_TRAJECTORY(self), 0.0);

    DJointTrajectory *joint = D_JOINT_TRAJECTORY(self);

    return joint->step_time;
}

static gdouble
d_joint_trajectory_calculate_move_time (DVector *deltaC,
                                        DVector *speed,
                                        gdouble accTime)
{
    gdouble values[4] = {
        abs(d_vector_get(deltaC, 0) / d_vector_get(speed, 0)),
        abs(d_vector_get(deltaC, 1) / d_vector_get(speed, 1)),
        abs(d_vector_get(deltaC, 2) / d_vector_get(speed, 2)),
        2.0 * accTime
    };
    g_message("deltaC: %f, %f, %f", d_vector_get(deltaC, 0),
                        d_vector_get(deltaC, 1),
                        d_vector_get(deltaC, 2));
    gdouble max = values[0];
    {
        int i;
        for(i = 1; i < 4; i++) {
            max = fmax (values[i-1], values[i]);
        }
    }
    g_message("d_joint_trajectory_calculate_move_time: move time: %f", max);
    return max;
}

static void
d_joint_trajectory_set_axes ()
{

}


/* Public API */
DJointTrajectory*
d_joint_trajectory_new (DVector *currentPosition,
                        DVector *currentDestination,
                        DVector *nextDestination,
                        DVector *maxSpeed)
{
    return d_joint_trajectory_new_full (currentPosition,
                                        currentDestination,
                                        nextDestination,
                                        maxSpeed,
                                        D_IT_DEFAULT_ACC_TIME,
                                        D_IT_DEFAULT_STEP_TIME);
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
    self = g_object_new (D_TYPE_JOINT_TRAJECTORY, NULL);

    self->time = -acceleration_time;
    self->acceleration_time = acceleration_time;

    self->current_axes = d_vector_clone(current_axes);

    self->move_destination = d_vector_clone(move_destination);

    self->start_speed = d_vector_clone(control_point);
    d_vector_sub(self->start_speed, current_axes);
    d_vector_scalar_mul(self->start_speed, 1.0 / acceleration_time);

    self->end_speed = d_vector_clone(move_destination);
    d_vector_sub(self->end_speed, control_point);

    self->move_time = d_joint_trajectory_calculate_move_time
                                                (self->end_speed,
                                                 max_speed,
                                                 acceleration_time);

    d_vector_scalar_mul(self->end_speed, 1.0/ self->move_time);

    self->control_point = d_vector_clone(control_point);

    self->step_time = step_time;

    return self;
}

