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

#include "dsim_trajectory.h"


/* #####   VARIABLES  -  LOCAL TO THIS SOURCE FILE   ################## */
struct _DLinearTrajectoryPrivate {
    /* private members of DLinearTrajectory */
    DVector     *pos;           /* Instant position */
    DVector     *cSpeed;        /* Cartesian Speed */
    DVector     *aSpeed;        /* Axes maximum speed */
    DVector     *deltaA;        /* Distance to current destination */
    DVector     *deltaC;        /* Distance from current/next destination */
    DVector     *pointB;        /* Current destination */
    gdouble     time;           /* Segment time starts at -accTime */
    gdouble     accTime;        /* Acceleration Time */
    gdouble     stepTime;       /* Step time for calculation */
    gdouble     moveTime;       /* Total movement time */
};

/* #####   PROTOTYPES  -  LOCAL TO THIS SOURCE FILE   ################# */
static void     d_linear_trajectory_interface_init  (DITrajectoryInterface  *iface);
static DVector* d_linear_trajectory_next            (DITrajectory           *self);

static void     d_itrajectory_interpolate_lspb      (DVector                *resPoint,
                                                     DVector                *deltaA,
                                                     DVector                *deltaC,
                                                     DVector                *pointB,
                                                     gdouble                moveTime,
                                                     gdouble                accelTime,
                                                     gdouble                segTime);

/* #####   DTRAJECTORYINTERFACE IMPLEMENTATION   ###################### */
G_DEFINE_INTERFACE(DITrajectory, d_itrajectory, G_TYPE_OBJECT);

static void
d_itrajectory_default_init(DITrajectoryInterface   *klass)
{
}

DVector*
d_trajectory_get_destination (DITrajectory *self)
{
    g_return_val_if_fail(D_IS_ITRAJECTORY(self), NULL);
    return D_ITRAJECTORY_GET_INTERFACE(self)->get_destination(self);
}

gboolean
d_trajectory_has_next (DITrajectory *self)
{
    g_return_val_if_fail(D_IS_ITRAJECTORY(self), FALSE);
    return D_ITRAJECTORY_GET_INTERFACE(self)->has_next(self);
}

DVector*
d_trajectory_next (DITrajectory     *self)
{
    g_return_val_if_fail (D_IS_ITRAJECTORY(self), NULL);
    return D_ITRAJECTORY_GET_INTERFACE(self)->next(self);
}

gdouble
d_trajectory_get_step_time (DITrajectory    *self)
{
    g_return_val_if_fail (D_IS_ITRAJECTORY(self), 0.0);
    return D_ITRAJECTORY_GET_INTERFACE(self)->get_step_time(self);
}

/* #####   DLINEARTRAJECTORY IMPLEMENTATION   ######################### */
G_DEFINE_TYPE_WITH_CODE (DLinearTrajectory,
                         d_linear_trajectory,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (D_TYPE_ITRAJECTORY,
                                                d_linear_trajectory_interface_init));

/* Create new DLinearTrajectory object */
DLinearTrajectory*
d_linear_trajectory_new (DVector    *currentPosition,
                         DVector    *currentDestination,
                         DVector    *nextDestination,
                         DVector     *aSpeed,
                         DVector     *cSpeed)
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
                              DVector    *aSpeed,
                              DVector    *cSpeed,
                              gdouble   accTime,
                              gdouble   stepTime)
{
    DLinearTrajectory* dlt;
    dlt = g_object_new (D_TYPE_LINEAR_TRAJECTORY, NULL);
    DLTPrivate *priv = D_LINEAR_TRAJECTORY_GET_PRIVATE(dlt);
    g_warning("d_linear_trajectory_new_full is a stub!");
    return dlt;
}

/* Dispose and finalize */
static void
d_linear_trajectory_dispose ( GObject    *obj )
{
    DLinearTrajectory *self = D_LINEAR_TRAJECTORY(obj);
    DLTPrivate *priv = D_LINEAR_TRAJECTORY_GET_PRIVATE(self);
    if (priv->pos) {
        g_object_unref(priv->pos);
        priv->pos = NULL;
    }
    if (priv->aSpeed) {
        g_object_unref(priv->aSpeed);
        priv->aSpeed = NULL;
    }
    if (priv->cSpeed) {
        g_object_unref(priv->cSpeed);
        priv->cSpeed = NULL;
    }
    if (priv->deltaA) {
        g_object_unref(priv->deltaA);
        priv->deltaA = NULL;
    }
    if (priv->pointB) {
        g_object_unref(priv->pointB);
        priv->pointB = NULL;
    }
    if (priv->deltaC) {
        g_object_unref(priv->deltaC);
        priv->deltaC = NULL;
    }
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
    DLTPrivate *priv;
    self->priv = priv = D_LINEAR_TRAJECTORY_GET_PRIVATE(self);
    /* Initialize private fields */
    priv->pos = NULL;
    priv->aSpeed = NULL;
    priv->cSpeed = NULL;
    priv->deltaA = NULL;
    priv->pointB = NULL;
    priv->deltaC = NULL;
    priv->time = 0.0;
    priv->accTime = 0.0;
    priv->moveTime = 0.0;
    priv->stepTime = 0.0;
}

static void
d_linear_trajectory_class_init ( DLinearTrajectoryClass   *klass )
{
    GObjectClass *goc = G_OBJECT_CLASS(klass);
    goc->dispose = d_linear_trajectory_dispose;
    goc->finalize = d_linear_trajectory_finalize;
    g_type_class_add_private(klass, sizeof(DLTPrivate));
}

static void
d_linear_trajectory_interface_init ( DITrajectoryInterface    *iface )
{
    iface->next = d_linear_trajectory_next;
}

/* Methods */
static DVector*
d_linear_trajectory_next (DITrajectory  *self)
{
    g_warning("d_linear_trajectory_next is a stub!!!");
    return d_pos_new();
}

