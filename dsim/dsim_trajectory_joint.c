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

/* Local Variables */
struct _DJointTrajectoryPrivate {
    /* private members of DJointTrajectory */
    DVector     *axes;          /* Instant position */
    DVector     *speed;         /* Axes Speed */
    DVector     *deltaA;        /* Distance to current destination */
    DVector     *deltaC;        /* Distance from current/next destination */
    DVector     *pointB;        /* Current destination */
    gdouble     time;           /* Segment Time starts at -accTime */
    gdouble     accTime;        /* Acceleration Time */
    gdouble     stepTime;       /* Step time for calculation */
    gdouble     moveTime;       /* Total Movement Time */
};

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
d_joint_trajectory_class_init ( DJointTrajectoryClass   *klass )
{
    GObjectClass *goc = G_OBJECT_CLASS(klass);
    goc->dispose = d_joint_trajectory_dispose;
    goc->finalize = d_joint_trajectory_finalize;
    g_type_class_add_private(klass, sizeof(DJTPrivate));
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
d_joint_trajectory_init ( DJointTrajectory  *self )
{
    DJTPrivate *priv;
    self->priv = priv = D_JOINT_TRAJECTORY_GET_PRIVATE(self);
    /* Initialize private fields */
    priv->axes = d_axes_new();
    priv->speed = d_speed_new();
    priv->deltaA = d_axes_new();
    priv->pointB = d_axes_new();
    priv->deltaC = d_axes_new();
    priv->time = 0.0;
    priv->accTime = 0.0;
    priv->moveTime = 0.0;
    priv->stepTime = 0.0;
}

static void
d_joint_trajectory_dispose ( GObject    *obj )
{
    DJointTrajectory *self = D_JOINT_TRAJECTORY(obj);
    if (self->priv->axes) {
        g_object_unref(self->priv->axes);
        self->priv->axes = NULL;
    }
    if (self->priv->speed) {
        g_object_unref(self->priv->speed);
        self->priv->speed = NULL;
    }
    if (self->priv->deltaA) {
        g_object_unref(self->priv->deltaA);
        self->priv->deltaA = NULL;
    }
    if (self->priv->pointB) {
        g_object_unref(self->priv->pointB);
        self->priv->pointB = NULL;
    }
    if (self->priv->deltaC) {
        g_object_unref(self->priv->deltaC);
        self->priv->deltaC = NULL;
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
    DJTPrivate *priv = D_JOINT_TRAJECTORY_GET_PRIVATE(joint);

    if (priv->time > priv->moveTime - priv->accTime) {
        return FALSE;
    }
    return TRUE;
}

static DVector*
d_joint_trajectory_get_destination (DITrajectory    *self)
{
    g_return_val_if_fail(D_IS_JOINT_TRAJECTORY(self), NULL);
    DJointTrajectory *joint = D_JOINT_TRAJECTORY(self);
    DJTPrivate *priv = D_JOINT_TRAJECTORY_GET_PRIVATE(joint);
    return priv->axes;
}

static DVector*
d_joint_trajectory_next (DITrajectory   *self)
{
    g_return_val_if_fail(D_IS_JOINT_TRAJECTORY(self), NULL);

    g_warning("d_joint_trajectory_next is a stub!!!");

    DJointTrajectory *joint = D_JOINT_TRAJECTORY(self);
    DJTPrivate *priv = D_JOINT_TRAJECTORY_GET_PRIVATE(joint);

    priv->time += priv->stepTime;
    d_joint_trajectory_interpolate_lspb(priv->axes,
                                   priv->deltaA,
                                   priv->deltaC,
                                   priv->pointB,
                                   priv->moveTime,
                                   priv->accTime,
                                   priv->time);

    return priv->axes;
}

static gdouble
d_joint_trajectory_get_step_time (DITrajectory  *self)
{
    g_return_val_if_fail(D_IS_JOINT_TRAJECTORY(self), 0.0);

    DJointTrajectory *joint = D_JOINT_TRAJECTORY(self);
    DJTPrivate *priv = D_JOINT_TRAJECTORY_GET_PRIVATE(joint);

    return priv->stepTime;
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
    DJointTrajectory* djt;
    djt = g_object_new (D_TYPE_JOINT_TRAJECTORY, NULL);
    DJTPrivate *priv = D_JOINT_TRAJECTORY_GET_PRIVATE(djt);

    d_vector_memcpy(priv->axes, currentPosition);
    d_vector_memcpy(priv->deltaA, currentDestination);
    d_vector_sub(priv->deltaA, currentPosition);

    d_vector_memcpy(priv->deltaC, nextDestination);
    d_vector_sub(priv->deltaC, currentDestination);

    d_vector_memcpy(priv->pointB, currentDestination);

    d_vector_memcpy(priv->speed, maxSpeed);

    priv->time = -accTime;
    priv->accTime = accTime;
    priv->stepTime = stepTime;
    priv->moveTime = d_joint_trajectory_calculate_move_time (priv->deltaC,
                                                             maxSpeed,
                                                             accTime);
    return djt;
}

