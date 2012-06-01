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
#include <dsim_pos.h>
#include <dsim_axes.h>
#include <dsim_speed.h>


/* ##########################  TRAJECTORY INTERFACE  ################### */
/*
 * Defines a DTrajectoryInterface with methods needed for both trajectory
 * types.
 */

/* DTrajectoryInterface macros */
#define D_TYPE_ITRAJECTORY                  (d_itrajectory_get_type ())
#define D_ITRAJECTORY(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_ITRAJECTORY, DITrajectory))
#define D_IS_ITRAJECTORY(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_ITRAJECTORY))
#define D_ITRAJECTORY_GET_INTERFACE(inst)   (G_TYPE_INSTANCE_GET_INTERFACE ((inst), D_TYPE_ITRAJECTORY, DITrajectoryInterface))

/* Interface structures */
typedef struct _DITrajectory            DITrajectory;  /* Dummy instance */

typedef struct _DITrajectoryInterface   DITrajectoryInterface;
struct _DITrajectoryInterface {
    GTypeInterface  parent;

    /* interface methods */
    DVector3*       (*next)         ( DITrajectory   *self );
};

/* Interface constants */
#define D_IT_DEFAULT_ACC_TIME 0.2
#define D_IT_DEFAULT_STEP_TIME  0.01

/* Interface get type */
GType       d_itrajectory_get_type  (void);

/* Interface methods default implementations */
DVector3*   d_trajectory_next       ( DITrajectory   *self );


/* #######################  LINEAR TRAJECTORY  ######################### */
/*
 * DLinearTrajectory implements DITrajectory Interface
 *
 * Creates a new robot trajectory in the cartesian space.
 */

/* Type DLinearTrajectory macros */
#define D_TYPE_LINEAR_TRAJECTORY               (d_linear_trajectory_get_type ())
#define D_LINEAR_TRAJECTORY(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_LINEAR_TRAJECTORY, DLinearTrajectory))
#define D_IS_LINEAR_TRAJECTORY(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_LINEAR_TRAJECTORY))
#define D_LINEAR_TRAJECTORY_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_LINEAR_TRAJECTORY, DLinearTrajectoryClass))
#define D_IS_LINEAR_TRAJECTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_LINEAR_TRAJECTORY))
#define D_LINEAR_TRAJECTORY_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_LINEAR_TRAJECTORY, DLinearTrajectoryClass))
#define D_LINEAR_TRAJECTORY_GET_PRIVATE(obj)   (G_TYPE_INSTANCE_GET_PRIVATE ((obj), D_TYPE_LINEAR_TRAJECTORY, DLTPrivate))


/* Instance Structure of DLinearTrajectory */
typedef struct _DLinearTrajectory           DLinearTrajectory;
typedef struct _DLinearTrajectoryPrivate    DLTPrivate;
struct _DLinearTrajectory {
    GObject         parent_instance;
    /* private */
    DLTPrivate      *priv;
};

/* Class Structure of DLinearTrajectory */
typedef struct _DLinearTrajectoryClass DLinearTrajectoryClass;
struct _DLinearTrajectoryClass {
    GObjectClass        parent_class;
};

/* Register DLinearTrajectory type */
GType   d_linear_trajectory_get_type    (void);

/* Methods */
DLinearTrajectory*  d_linear_trajectory_new         (DPos      *currentPosition,
                                                     DPos      *currentDestination,
                                                     DPos      *nextDestination,
                                                     DSpeed    *aSpeed,
                                                     DSpeed    *cSpeed );

DLinearTrajectory*  d_linear_trajectory_new_full    (DPos      *currentPosition,
                                                     DPos      *currentDestination,
                                                     DPos      *nextDestination,
                                                     DSpeed    *aSpeed,
                                                     DSpeed    *cSpeed,
                                                     gdouble   accTime,
                                                     gdouble   stepTime );


/* #######################  JOINT TRAJECTORY  ########################## */
/*
 * DJointTrajectory implements DITrajectory Interface
 *
 * Creates a new robot trajectory in the joint angles space.
 */

/* Type DJointTrajectory macros */
#define D_TYPE_JOINT_TRAJECTORY               (d_joint_trajectory_get_type ())
#define D_JOINT_TRAJECTORY(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_JOINT_TRAJECTORY, DJointTrajectory))
#define D_IS_JOINT_TRAJECTORY(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_JOINT_TRAJECTORY))
#define D_JOINT_TRAJECTORY_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_JOINT_TRAJECTORY, DJointTrajectoryClass))
#define D_IS_JOINT_TRAJECTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_JOINT_TRAJECTORY))
#define D_JOINT_TRAJECTORY_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_JOINT_TRAJECTORY, DJointTrajectoryClass))
#define D_JOINT_TRAJECTORY_GET_PRIVATE(obj)   (G_TYPE_INSTANCE_GET_PRIVATE ((obj), D_TYPE_JOINT_TRAJECTORY, DJTPrivate))

/* Instance Structure of DJointTrajectory */
typedef struct _DJointTrajectory           DJointTrajectory;
typedef struct _DJointTrajectoryPrivate    DJTPrivate;
struct _DJointTrajectory {
    GObject         parent_instance;
    /* private */
    DJTPrivate      *priv;
};

/* Class Structure of DJointTrajectory */
typedef struct _DJointTrajectoryClass DJointTrajectoryClass;
struct _DJointTrajectoryClass {
    GObjectClass        parent_class;
};

/* Register DJointTrajectory type */
GType   d_joint_trajectory_get_type     (void);

/* Methods */
DJointTrajectory*   d_joint_trajectory_new          (DAxes     *currentPosition,
                                                     DAxes     *currentDestination,
                                                     DAxes     *nextDestination,
                                                     DSpeed    *maxSpeed );

DJointTrajectory*   d_joint_trajectory_new_full     (DAxes     *currentPosition,
                                                     DAxes     *currentDestination,
                                                     DAxes     *nextDestination,
                                                     DSpeed    *maxSpeed,
                                                     gdouble   accTime,
                                                     gdouble   stepTime );

#endif   /* ----- #ifndef DSIM_TRAJ_INC  ----- */
