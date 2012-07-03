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
 * dsim_trajectory.h : Trajectory dispatcher for the Delta Robot Simulator
 */

#ifndef  DSIM_TRAJ_INC
#define  DSIM_TRAJ_INC

#include <glib-object.h>
#include <math.h>
#include "dsim_vector.h"
#include "dsim_pos.h"
#include "dsim_axes.h"
#include "dsim_speed.h"
#include "dsim_solver.h"


/* #######################  TRAJECTORY CONTROL ORDERS  #################### */
/*
 * Specification of orders to be executed by DTrajectoryControl
 */

/* Type DTrajectoryCommand macros */
#define D_TYPE_TRAJECTORY_COMMAND               (d_trajectory_command_get_type ())
#define D_TRAJECTORY_COMMAND(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_TRAJECTORY_COMMAND, DTrajectoryCommand))
#define D_IS_TRAJECTORY_COMMAND(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_TRAJECTORY_COMMAND))
#define D_TRAJECTORY_COMMAND_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_TRAJECTORY_COMMAND, DTrajectoryCommandClass))
#define D_IS_TRAJECTORY_COMMAND_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_TRAJECTORY_COMMAND))
#define D_TRAJECTORY_COMMAND_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_TRAJECTORY_COMMAND, DTrajectoryCommandClass))

typedef enum DCommandType{
    OT_MOVEJ,
    OT_MOVEL,
    OT_WAIT,
    OT_END,
} DCommandType;

/* Instance Structure of DTrajectoryCommand */
typedef struct _DTrajectoryCommand DTrajectoryCommand;
struct _DTrajectoryCommand {
    GObject         parent_instance;

    DCommandType    command_type;
    gpointer        data;
};

/* Class Structure of DTrajectoryCommand */
typedef struct _DTrajectoryCommandClass DTrajectoryCommandClass;
struct _DTrajectoryCommandClass {
    GObjectClass        parent_class;
};

/* Register DTrajectoryCommand type */
GType               d_trajectory_command_get_type       (void);

DTrajectoryCommand* d_trajectory_command_new            (DCommandType   cmdt,
                                                         gpointer       data);

/* ##########################  TRAJECTORY CONTROL  ######################*/
/*
 * Defines a DTrajectoryControl object. A singleton multithreaded real-time dispatcher
 * for controling trajectories
 */
#define D_TYPE_TRAJECTORY_CONTROL               (d_trajectory_control_get_type ())
#define D_TRAJECTORY_CONTROL(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_TRAJECTORY_CONTROL, DTrajectoryControl))
#define D_IS_TRAJECTORY_CONTROL(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_TRAJECTORY_CONTROL))
#define D_TRAJECTORY_CONTROL_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_TRAJECTORY_CONTROL, DTrajectoryControlClass))
#define D_IS_TRAJECTORY_CONTROL_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_TRAJECTORY_CONTROL))
#define D_TRAJECTORY_CONTROL_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_TRAJECTORY_CONTROL, DTrajectoryControlClass))

typedef void (*DTrajectoryOutputFunc) (DVector *axes, gpointer data);

typedef struct _DTrajectoryControl DTrajectoryControl;
struct _DTrajectoryControl {
    GObject         parent_instence;

    /* Geometry of the manipulator */
    DGeometry       *geometry;

    /* List of destinations */
    GAsyncQueue     *orders;

    /* Absolute positions in work space */
    DVector         *current_destination;
    DVector         *current_position;

    /* Absolute positions in axes space */
    DVector         *current_destination_axes;
    DVector         *current_position_axes;

    /* Acceleration time for both ends */
    gdouble         accelTime;
    gdouble         decelTime;      /* Set equal to accelTime for now */

    /* Max Speed in the axes space */
    DVector         *max_speed;

    /* Step time for trajectory, used for timers */
    gdouble         stepTime;

    /* Thread control */
    gboolean        exit_flag;
    GThread         *main_loop;

    /* Output function */
    DTrajectoryOutputFunc   joint_out_fun;
    gpointer                joint_out_data;

    /* Output function for linear trajectories */
    DTrajectoryOutputFunc   linear_out_fun;
    gpointer                linear_out_data;
};

typedef struct _DTrajectoryControlClass DTrajectoryControlClass;
struct _DTrajectoryControlClass {
    GObjectClass    parent_class;
};

GType               d_trajectory_control_get_type   (void);

DTrajectoryControl* d_trajectory_control_new        (void);

void                d_trajectory_control_push_order (DTrajectoryControl *self,
                                                     DTrajectoryCommand *order);

void                d_trajectory_control_start      (DTrajectoryControl *self);

void                d_trajectory_control_stop       (DTrajectoryControl *self);

void                d_trajectory_control_set_output_func (DTrajectoryControl    *self,
                                                     DTrajectoryOutputFunc  func,
                                                     gpointer               output_data);

/* #######################  COMMON TRAJECTORY SUPER CLASS  ############# */
/**
 * DTrajectory provides a single interface for trajectories. Serves as a
 * superclass for both DLinearTrajectory and DJointTrajectory.
 * Is intended to be a point generator by calling next() at regular step time.
 * The algorithm used for generating points is Linear Segment with Parabolic
 * Blends given control points. TODO:Make this algorithm overridable.
 * The semantics of positioning vectors and speeds are defined by subclasses.
 */

/* Type macros */
#define D_TYPE_TRAJECTORY               (d_trajectory_get_type ())
#define D_TRAJECTORY(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_TRAJECTORY, DTrajectory))
#define D_IS_TRAJECTORY(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_TRAJECTORY))
#define D_TRAJECTORY_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_TRAJECTORY, DTrajectoryClass))
#define D_IS_TRAJECTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_TRAJECTORY))
#define D_TRAJECTORY_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_TRAJECTORY, DTrajectoryClass))

#define D_DEFAULT_ACC_TIME 0.2
#define D_DEFAULT_STEP_TIME  0.01
/* Instance structure of DTrajectory */
typedef struct _DTrajectory DTrajectory;
struct _DTrajectory {
    GObject         parent_object;

    /* private */
    /* Time variable */
    gdouble         time;

    /* Time constants */
    gdouble         move_time;
    gdouble         acceleration_time;
    gdouble         step_time;

    /* Postion variable */
    DVector         *current;

    /* Destination point */
    DVector         *destination;

    /* Blend start/end speed */
    DVector         *start_speed;
    DVector         *end_speed;

    /* Control point around the speed blend */
    DVector         *control_point;
};

/* Class structure of DTrajectory */
typedef struct _DTrajectoryClass DTrajectoryClass;
struct _DTrajectoryClass {
    GObjectClass    parent_class;

    /* Private virtual methods */
    void            (*interpolate_fun)  (DTrajectory    *self);

    /* Public virtual methods */
    DVector*        (*get_destination)  (DTrajectory    *self);
    gboolean        (*has_next)         (DTrajectory    *self);
    DVector*        (*next)             (DTrajectory    *self);
    gdouble         (*get_step_time)    (DTrajectory    *self);
};

/* Methods */
GType       d_trajectory_get_type           (void);

gboolean    d_trajectory_has_next           (DTrajectory    *self);

DVector*    d_trajectory_next               (DTrajectory    *self);

gdouble     d_trajectory_get_step_time      (DTrajectory    *self);

DVector*    d_trajectory_get_destination    (DTrajectory    *self);

gdouble     d_trajectory_calculate_move_time(DVector        *displacement,
                                             DSpeed         *speed,
                                             gdouble        acceleration_time);

//void        d_trajectory_interpolate_lspb   (DVector        *res_point,
//                                             DVector        *start_speed,
//                                             DVector        *end_speed,
//                                             DVector        *control_point,
//                                             gdouble        acceleration_time,
//                                             gdouble        time);

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

/* Instance Structure of DLinearTrajectory */
typedef struct _DLinearTrajectory           DLinearTrajectory;
struct _DLinearTrajectory {
    GObject         parent_instance;

    /* private */
    /* Instant position */
    DVector     *current_pos;

    /* Current destination */
    DVector     *move_destination;

    /* Start and end speeds (end_speed = cartesian_speed) */
    DVector     *start_speed;
    DVector     *end_speed;

    /* Axes maximum speed, currently not checked */
    DVector     *axes_speed;

    /* Control point (current destination) */
    DVector     *control_point;

    /* Time constants for this movement */
    gdouble     acceleration_time;
    gdouble     step_time;

    /* Move time estimated */
    gdouble     move_time;

    /* Segment time counter (starts at -accTime) */
    gdouble     time;
};

/* Class Structure of DLinearTrajectory */
typedef struct _DLinearTrajectoryClass DLinearTrajectoryClass;
struct _DLinearTrajectoryClass {
    GObjectClass        parent_class;
};

/* Register DLinearTrajectory type */
GType               d_linear_trajectory_get_type    (void);

/* Methods */
DLinearTrajectory*  d_linear_trajectory_new         (DVector    *currentPosition,
                                                     DVector    *currentDestination,
                                                     DVector    *nextDestination,
                                                     DVector     *aSpeed,
                                                     DVector     *cSpeed);

DLinearTrajectory*  d_linear_trajectory_new_full    (DVector    *currentPosition,
                                                     DVector    *currentDestination,
                                                     DVector    *nextDestination,
                                                     DVector     *aSpeed,
                                                     DVector     *cSpeed,
                                                     gdouble    accTime,
                                                     gdouble    stepTime);


/* #######################  JOINT TRAJECTORY  ########################## */
/*
 * DJointTrajectory subclass of DTrajectory. Positions and speeds are given in
 * the axes space.
 */

/* Type DJointTrajectory macros */
#define D_TYPE_JOINT_TRAJECTORY               (d_joint_trajectory_get_type ())
#define D_JOINT_TRAJECTORY(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_JOINT_TRAJECTORY, DJointTrajectory))
#define D_IS_JOINT_TRAJECTORY(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_JOINT_TRAJECTORY))
#define D_JOINT_TRAJECTORY_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_JOINT_TRAJECTORY, DJointTrajectoryClass))
#define D_IS_JOINT_TRAJECTORY_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_JOINT_TRAJECTORY))
#define D_JOINT_TRAJECTORY_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_JOINT_TRAJECTORY, DJointTrajectoryClass))

/* Instance Structure of DJointTrajectory */
typedef struct _DJointTrajectory           DJointTrajectory;
struct _DJointTrajectory {
    DTrajectory         parent_instance;
};

/* Class Structure of DJointTrajectory */
typedef struct _DJointTrajectoryClass DJointTrajectoryClass;
struct _DJointTrajectoryClass {
    DTrajectoryClass    parent_class;
};

/* Register DJointTrajectory type */
GType               d_joint_trajectory_get_type     (void);

/* Methods */
DJointTrajectory*   d_joint_trajectory_new          (DAxes      *current_axes,
                                                     DAxes      *control_point,
                                                     DAxes      *move_destination,
                                                     DSpeed     *max_speed);

DJointTrajectory*   d_joint_trajectory_new_full     (DAxes      *current_axes,
                                                     DAxes      *control_point,
                                                     DAxes      *move_destination,
                                                     DSpeed     *max_speed,
                                                     gdouble    acceleration_time,
                                                     gdouble    step_time);

#endif   /* ----- #ifndef DSIM_TRAJ_INC  ----- */
