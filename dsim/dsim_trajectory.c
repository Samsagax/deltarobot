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

/* Auxiliary functions */
static gdouble
getMoveTime ( DTrajectory* self )
{
    gdouble values[4] = {
        abs(d_vector3_get(self->deltaC, 0)),
        abs(d_vector3_get(self->deltaC, 1)),
        abs(d_vector3_get(self->deltaC, 2)),
        self->accelTime
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

/* Virtual Methods */
DVector3*
d_trajectory_next ( DTrajectory* self )
{
    return D_TRAJECTORY_GET_CLASS(self)->next();
}

static DVector3*
d_real_default_linear_next ( void )
{
    //TODO: interpolate and give next point
}

static DVector3*
d_real_default_joint_next ( void )
{
    //TODO: interpolate and give next point

}

/* GtypeRegister */
G_DEFINE_TYPE(DTrajectory, d_trajectory, G_TYPE_OBJECT);

/* Constructors */
static DTrajectory*
d_trajectory_new ( DTrajectoryType      type,
                   DTrajInterpolator    interpolator )
{
    DTrajectory* instance = D_TRAJECTORY(g_object_new(D_TYPE_TRAJECTORY, NULL));
    D_TRAJECTORY_GET_CLASS(instance)->next = interpolator;
    instance->type = type;
}

DTrajectory*
d_trajectory_new_linear ( void )
{
    return d_trajectory_new (D_TRAJECTORY_LINEAR, d_real_default_linear_next);
}

DTrajectory*
d_trajectory_new_joint ( void )
{
    return d_trajectory_new (D_TRAJECTORY_JOINT, d_real_default_joint_next);
}

/* Destructor */
static void
d_trajectory_dispose ( GObject* object )
{
    DTrajectory* self = D_TRAJECTORY(object);
    if (self->currentDestination){
        g_object_unref(self->currentPosition);
        self->currentPosition = NULL;
    }
    if (self->currentDestination) {
        g_object_unref(self->currentDestination);
        self->currentDestination = NULL;
    }
    if (self->nextDestination) {
        g_object_unref(self->nextDestination);
        self->nextDestination = NULL;
    }
    G_OBJECT_CLASS (d_trajectory_parent_class)->dispose(object);
}

static void
d_trajectory_finalize ( GObject* object )
{
    G_OBJECT_CLASS (d_trajectory_parent_class)->finalize(object);
}

/* Init class and instence */
static void
d_trajectory_class_init ( DTrajectoryClass* klass )
{
    GObjectClass* oclass = G_OBJECT_CLASS(klass);
    oclass->dispose =   d_trajectory_dispose;
    oclass->finalize =  d_trajectory_finalize;
    klass->next =       d_real_default_joint_next; //TODO: Check This setting
}

static void
d_trajectory_init ( DTrajectory* self)
{
    self->type = D_TRAJECTORY_JOINT;
    self->currentPosition =     d_vector3_new();
    self->currentDestination =  d_vector3_new();
    self->nextDestination =     d_vector3_new();
    self->stepTime =            0.01;
    self->accelTime =           0.1;
    self->segTime =             -(self->accelTime);
}

