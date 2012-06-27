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
d_linear_trajectory_interface_init  (DITrajectoryInterface  *iface);

static void
d_linear_trajectory_class_init      (DLinearTrajectoryClass *klass);

static void
d_linear_trajectory_init            (DLinearTrajectory      *self);

static void
d_linear_trajectory_dispose         (GObject                *obj);

static void
d_linear_trajectory_finalize        (GObject                *obj);

static DVector*
d_linear_trajectory_next            (DITrajectory           *self);

/* Implementation Internals */
G_DEFINE_TYPE_WITH_CODE (DLinearTrajectory,
                         d_linear_trajectory,
                         G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (D_TYPE_ITRAJECTORY,
                                                d_linear_trajectory_interface_init));

static void
d_linear_trajectory_interface_init (DITrajectoryInterface   *iface)
{
    iface->next = d_linear_trajectory_next;
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
    g_warning("d_linear_trajectory_init is a stub");

    self->pos = NULL;
    self->aSpeed = NULL;
    self->cSpeed = NULL;
    self->deltaA = NULL;
    self->pointB = NULL;
    self->deltaC = NULL;
    self->time = 0.0;
    self->accTime = 0.0;
    self->moveTime = 0.0;
    self->stepTime = 0.0;
}

static void
d_linear_trajectory_dispose (GObject    *obj)
{
    g_warning("d_linear_trajectory_dispose is a stub");

    DLinearTrajectory *self = D_LINEAR_TRAJECTORY(obj);
    if (self->pos) {
        g_object_unref(self->pos);
        self->pos = NULL;
    }
    if (self->aSpeed) {
        g_object_unref(self->aSpeed);
        self->aSpeed = NULL;
    }
    if (self->cSpeed) {
        g_object_unref(self->cSpeed);
        self->cSpeed = NULL;
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
    G_OBJECT_CLASS(d_linear_trajectory_parent_class)->dispose(obj);
}

static void
d_linear_trajectory_finalize (GObject   *obj)
{
    /* Chain up */
    G_OBJECT_CLASS(d_linear_trajectory_parent_class)->finalize(obj);
}

static DVector*
d_linear_trajectory_next (DITrajectory  *self)
{
    g_warning("d_linear_trajectory_next is a stub!!!");
    return d_pos_new();
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

