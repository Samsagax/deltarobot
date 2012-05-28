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
 * dsim_trajectory.c :
 */

#include <stdlib.h>
#include <math.h>
#include "dsim_trajectory.h"


/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################## */
struct _DLinearTrajectoryPrivate {
    DVector3    *dummy;/* private members of DLinearTrajectory */
};

struct _DJointTrajectoryPrivate {
    /* private members of DJointTrajectory */
    DAxes       *axes;          /* Instant position */
    DSpeed      *speed;         /* Axes Speed */
    DAxes       *deltaA;        /* Distance to current destination */
    DAxes       *deltaC;        /* Distance from current/next destination */
    DAxes       *pointB;        /* Current destination */
    gdouble     time;           /* Segment Time starts at -accTime */
    gdouble     accTime;        /* Acceleration Time */
    gdouble     stepTime;       /* Step time for calculation */
    gdouble     moveTime;       /* Total Movement Time */
};

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ################# */
static void     d_linear_trajectory_interface_init  (DITrajectoryInterface * iface);
static DVector3*    d_linear_trajectory_next        (DITrajectory  *self);

static void     d_joint_trajectory_interface_init   (DITrajectoryInterface * iface);
static DVector3*    d_joint_trajectory_next         (DITrajectory  *self);

static gdouble      calculateMoveTime           ( DVector3      *deltaC,
                                                  DVector3      *speed,
                                                  gdouble       accTime );
static void         interpolateLSB              ( DVector3*  resPoint,
                                                  DVector3*  deltaA,
                                                  DVector3*  deltaC,
                                                  DVector3*  pointB,
                                                  gdouble    moveTime,
                                                  gdouble    accelTime,
                                                  gdouble    segTime );

/* #####   DTRAJECTORYINTERFACE IMPLEMENTATION   ###################### */
G_DEFINE_INTERFACE(DITrajectory, d_itrajectory, G_TYPE_OBJECT);

static void d_itrajectory_default_init (DITrajectoryInterface *klass)
{
}

DVector3*
d_trajectory_next ( DITrajectory   *self )
{
    g_return_val_if_fail (D_IS_ITRAJECTORY(self), NULL);
    return D_ITRAJECTORY_GET_INTERFACE(self)->next(self);
}

/* #####   DLINEARTRAJECTORY IMPLEMENTATION   ######################### */
G_DEFINE_TYPE_WITH_CODE ( DLinearTrajectory,
                          d_linear_trajectory,
                          G_TYPE_OBJECT,
                          G_IMPLEMENT_INTERFACE ( D_TYPE_ITRAJECTORY,
                                                  d_linear_trajectory_interface_init));

/* Create new DLinearTrajectory object */
DLinearTrajectory*
d_linear_trajectory_new  (void)
{
    DLinearTrajectory* djt;
    djt = g_object_new ( D_TYPE_LINEAR_TRAJECTORY, NULL );
    return djt;
}

/* Dispose and finalize */
static void
d_linear_trajectory_dispose ( GObject    *obj )
{
    /* Chain up */
    G_OBJECT_CLASS(d_linear_trajectory_parent_class)->dispose(obj);
}
static void
d_linear_trajectory_finalize ( GObject   *obj )
{
    /* Chain up */
    G_OBJECT_CLASS(d_linear_trajectory_parent_class)->finalize(obj);
}

/* Init functions */
static void
d_linear_trajectory_init ( DLinearTrajectory  *self )
{

}

static void
d_linear_trajectory_class_init ( DLinearTrajectoryClass   *klass )
{
    GObjectClass *goc = G_OBJECT_CLASS(klass);
    goc->dispose = d_linear_trajectory_dispose;
    goc->finalize = d_linear_trajectory_finalize;
}

static void
d_linear_trajectory_interface_init ( DITrajectoryInterface    *iface )
{
    iface->next = d_linear_trajectory_next;
}

/* Methods */
static DVector3*
d_linear_trajectory_next (DITrajectory  *self)
{
    g_warning("d_linear_trajectory_next is a stub!!!");
    return d_pos_new();
}

/* #####   DJOINTTRAJECTORY IMPLEMENTATION   ########################## */
G_DEFINE_TYPE_WITH_CODE ( DJointTrajectory,
                          d_joint_trajectory,
                          G_TYPE_OBJECT,
                          G_IMPLEMENT_INTERFACE ( D_TYPE_ITRAJECTORY,
                                                  d_joint_trajectory_interface_init));

/* Create new DJointTrajectory object */
DJointTrajectory*
d_joint_trajectory_new ( DAxes      *currentPosition,
                         DAxes      *currentDestination,
                         DAxes      *nextDestination,
                         DSpeed     *maxSpeed )
{
    return d_joint_trajectory_new_full ( currentPosition,
                                         currentDestination,
                                         nextDestination,
                                         maxSpeed,
                                         D_IT_DEFAULT_ACC_TIME,
                                         D_IT_DEFAULT_STEP_TIME );
}

DJointTrajectory*
d_joint_trajectory_new_full ( DAxes     *currentPosition,
                              DAxes     *currentDestination,
                              DAxes     *nextDestination,
                              DSpeed    *maxSpeed,
                              gdouble   accTime,
                              gdouble   stepTime )
{
    DJointTrajectory* djt;
    djt = g_object_new ( D_TYPE_JOINT_TRAJECTORY, NULL );
    DJTPrivate *priv = D_JOINT_TRAJECTORY_GET_PRIVATE(djt);

    priv->axes = d_axes_copy(currentPosition);
    priv->deltaA = d_axes_copy(currentDestination);
    d_axes_substract(priv->deltaA, currentPosition);

    priv->deltaC = d_axes_copy(nextDestination);
    d_axes_substract(priv->deltaC, currentDestination);

    priv->pointB = d_axes_copy(currentDestination);

    priv->speed = maxSpeed;
    g_object_ref(maxSpeed);

    priv->time = -accTime;
    priv->accTime = accTime;
    priv->stepTime = stepTime;
    priv->moveTime = calculateMoveTime ( D_VECTOR3(priv->deltaC),
                                         D_VECTOR3(maxSpeed),
                                         accTime);
    return djt;
}

/* Dispose and finalize */
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

/* Init functions */
static void
d_joint_trajectory_init ( DJointTrajectory  *self )
{
    DJTPrivate *priv;
    self->priv = priv = D_JOINT_TRAJECTORY_GET_PRIVATE(self);
    /* Initialize private fields */
    priv->axes = NULL;
    priv->speed = NULL;
    priv->deltaA = NULL;
    priv->pointB = NULL;
    priv->deltaC = NULL;
    priv->time = 0.0;
    priv->accTime = 0.0;
    priv->moveTime = 0.0;
    priv->stepTime = 0.0;
}

static void
d_joint_trajectory_class_init ( DJointTrajectoryClass   *klass )
{
    GObjectClass *goc = G_OBJECT_CLASS(klass);
    goc->dispose = d_joint_trajectory_dispose;
    goc->finalize = d_joint_trajectory_finalize;
    g_type_class_add_private(klass, sizeof(DJTPrivate));
}

static void
d_joint_trajectory_interface_init ( DITrajectoryInterface    *iface )
{
    iface->next = d_joint_trajectory_next;
}

/* Methods */
static DVector3*
d_joint_trajectory_next ( DITrajectory     *self )
{
    if (!D_IS_JOINT_TRAJECTORY(self)) {
        g_error("d_joint_trajectory_next must be called on a DJointTrajectory object");
    }
    DJointTrajectory *joint = D_JOINT_TRAJECTORY(self);
    DJTPrivate *priv = D_JOINT_TRAJECTORY_GET_PRIVATE(joint);
    priv->time += priv->stepTime;
    interpolateLSB( D_VECTOR3(priv->axes),
                    D_VECTOR3(priv->deltaA),
                    D_VECTOR3(priv->deltaC),
                    D_VECTOR3(priv->pointB),
                    priv->moveTime,
                    priv->accTime,
                    priv->time );
    g_warning("d_joint_trajectory_next is a stub!!!");
    return d_axes_copy(D_VECTOR3(priv->axes));
}

// ############ OLD CODE ############
/* Auxiliary functions */
static gdouble
calculateMoveTime ( DVector3    *deltaC,
                    DVector3    *speed,
                    gdouble     accTime )
{
    gdouble values[4] = {
        abs(d_vector3_get(deltaC, 0) / d_vector3_get(speed, 0)),
        abs(d_vector3_get(deltaC, 1) / d_vector3_get(speed, 1)),
        abs(d_vector3_get(deltaC, 2) / d_vector3_get(speed, 2)),
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
interpolateLSB ( DVector3*  resPoint,
                 DVector3*  deltaA,
                 DVector3*  deltaC,
                 DVector3*  pointB,
                 gdouble    moveTime,
                 gdouble    accelTime,
                 gdouble    segTime )
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
            d_vector3_set(resPoint, i,
                      d_vector3_get(pointB, i)
                    + d_vector3_get(deltaC, i) * tfactC
                    + d_vector3_get(deltaA, i) * tfactA);
        }
    }
}

