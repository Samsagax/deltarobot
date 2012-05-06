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
 * dsim_traj.h : Trajectory dispatcher for the Delta Robot Simulator
 */

#ifndef  DSIM_TRAJ_INC
#define  DSIM_TRAJ_INC

#include <glib-object.h>
#include <dsim/dsim_vector.h>

/* Type macros */
#define D_TYPE_TRAJECTORY               (d_trajectory_get_type ())
#define D_TRAJECTORY(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_TRAJECTORY, DTrajectory))
#define D_IS_TRAJECTORY(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_TRAJECTORY))
#define D_TRAJECTORY_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_TRAJECTORY, DTrajectoryClass))
#define D_IS_TRAJECTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_TRAJECTORY))
#define D_TRAJECTORY_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_TRAJECTORY, DTrajectoryClass))

/* Define a flag for the trajectory type */
typedef enum {
    D_TRAJECTORY_JOINT,
    D_TRAJECTORY_LINEAR
} DTrajectoryType;

/* Define a trajectory interpolation function */
typedef DVector3*  (*DTrajInterpolator)    (void);

/* Instance Structure of DTrajectory */
typedef struct _DTrajectory DTrajectory;
struct _DTrajectory {
    GObject         parent_instance;
    DTrajectoryType type;

    /* private */
    DVector3*       currentPosition;
    DVector3*       currentDestination;
    DVector3*       nextDestination;
    DVector3*       deltaA;
    DVector3*       deltaC;
    gdouble         stepTime;
    gdouble         accelTime;
    gdouble         segTime;
};

/* Class Structure of DTrajectory */
typedef struct _DTrajectoryClass DTrajectoryClass;
struct _DTrajectoryClass {
    GObjectClass        parent_class;

    /* Virtual Methods */
    DTrajInterpolator   next;
};

/* Returns GType associated with this object type */
GType           d_trajectory_get_type       (void);

/* Get Trajectory instance */
DTrajectory*    d_trajectory_new_linear     (void);
DTrajectory*    d_trajectory_new_joint      (void);

/* Methods */
void            d_trajectory_new_dest       (DVector3*      point);
DVector3*       d_trajectory_next           (DTrajectory*   self);

//OLD
//void moveJoint( DVector3 destination );
#endif   /* ----- #ifndef DSIM_TRAJ_INC  ----- */
