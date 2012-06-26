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

static void
d_joint_trajectory_interpolate_lspb     (DVector                *resPoint,
                                         DVector                *deltaA,
                                         DVector                *deltaC,
                                         DVector                *pointB,
                                         gdouble                moveTime,
                                         gdouble                accelTime,
                                         gdouble                segTime);

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
    self->axes = NULL;
    self->speed = NULL;
    self->deltaA = NULL;
    self->pointB = NULL;
    self->deltaC = NULL;
    self->time = 0.0;
    self->accTime = 0.0;
    self->moveTime = 0.0;
    self->stepTime = 0.0;
}

static void
d_joint_trajectory_dispose (GObject    *obj)
{
    DJointTrajectory *self = D_JOINT_TRAJECTORY(obj);
    if (self->axes) {
        g_object_unref(self->axes);
        self->axes = NULL;
    }
    if (self->speed) {
        g_object_unref(self->speed);
        self->speed = NULL;
    }
    if (self->deltaA) {
        g_object_unref(self->deltaA);
        self->deltaA = NULL;
    }
    if (self->pointB) {
        g_object_unref(self->pointB);
        self->pointB = NULL;
    }
    if (self->deltaC) {
        g_object_unref(self->deltaC);
        self->deltaC = NULL;
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

    if (joint->time > joint->moveTime - joint->accTime) {
        return FALSE;
    }
    return TRUE;
}

static DVector*
d_joint_trajectory_get_destination (DITrajectory    *self)
{
    g_return_val_if_fail(D_IS_JOINT_TRAJECTORY(self), NULL);

    DJointTrajectory *joint = D_JOINT_TRAJECTORY(self);

    return joint->axes;
}

static DVector*
d_joint_trajectory_next (DITrajectory   *self)
{
    g_return_val_if_fail(D_IS_JOINT_TRAJECTORY(self), NULL);

    g_warning("d_joint_trajectory_next is a stub!!!");

    DJointTrajectory *joint = D_JOINT_TRAJECTORY(self);

    joint->time += joint->stepTime;
    d_joint_trajectory_interpolate_lspb(joint->axes,
                                   joint->deltaA,
                                   joint->deltaC,
                                   joint->pointB,
                                   joint->moveTime,
                                   joint->accTime,
                                   joint->time);

    return joint->axes;
}

static gdouble
d_joint_trajectory_get_step_time (DITrajectory  *self)
{
    g_return_val_if_fail(D_IS_JOINT_TRAJECTORY(self), 0.0);

    DJointTrajectory *joint = D_JOINT_TRAJECTORY(self);

    return joint->stepTime;
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
    gdouble max = values[0];
    {
        int i;
        for(i = 1; i < 4; i++) {
            max = fmax (values[i-1], values[i]);
        }
    }
    return max;
}
static void
d_joint_trajectory_set_axes ()
{

}

static void
d_joint_trajectory_interpolate_lspb (DVector *resPoint,
                                DVector *deltaA,
                                DVector *deltaC,
                                DVector *pointB,
                                gdouble moveTime,
                                gdouble accelTime,
                                gdouble segTime)
{
    gdouble tfactC, tfactA;
    if (segTime < accelTime) {
        tfactC = pow(segTime + accelTime, 2.0)
                        / (4.0 * accelTime * moveTime);
        tfactA = pow(segTime - accelTime, 2.0)
                        / (4.0 * accelTime * accelTime);
    } else {
        tfactC = segTime / moveTime;
        tfactA = 0.0;
    }
    {
        int i;
        for (i = 0; i < 3; i++) {
            d_vector_set(resPoint, i,
                      d_vector_get(pointB, i)
                    + d_vector_get(deltaC, i) * tfactC
                    + d_vector_get(deltaA, i) * tfactA);
        }
    }
}

/* Public API */
DJointTrajectory*
d_joint_trajectory_new (DVector *currentPosition,
                        DVector *currentDestination,
                        DVector *nextDestination,
                        DVector *maxSpeed)
{
    return d_joint_trajectory_new_full ( currentPosition,
                                         currentDestination,
                                         nextDestination,
                                         maxSpeed,
                                         D_IT_DEFAULT_ACC_TIME,
                                         D_IT_DEFAULT_STEP_TIME );
}

DJointTrajectory*
d_joint_trajectory_new_full (DVector    *currentPosition,
                             DVector    *currentDestination,
                             DVector    *nextDestination,
                             DVector    *maxSpeed,
                             gdouble    accTime,
                             gdouble    stepTime)
{
    DJointTrajectory *self;
    self = g_object_new (D_TYPE_JOINT_TRAJECTORY, NULL);

    self->axes = d_vector_clone(currentPosition);

    self->deltaA = d_vector_clone(currentDestination);
    d_vector_sub(self->deltaA, currentPosition);

    self->deltaC = d_vector_clone(nextDestination);
    d_vector_sub(self->deltaC, currentDestination);

    self->pointB = d_vector_clone(currentDestination);

    self->speed = d_vector_clone(maxSpeed);

    self->time = -accTime;
    self->accTime = accTime;
    self->stepTime = stepTime;
    self->moveTime = d_joint_trajectory_calculate_move_time (self->deltaC,
                                                             maxSpeed,
                                                             accTime);
    return self;
}

