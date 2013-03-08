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
 * dsim_trajectory_control.c :
 */

#include "dsim_trajectory.h"

//#include <signal.h>
//#include <time.h>

#define D_NANOS_PER_SEC 1000000000

/* Local variables */
static GMutex   timer_mutex;
static GCond    wakeup_cond;

/* Forward declarations */
static void     d_trajectory_control_class_init
                        (DTrajectoryControlClass    *klass);

static void     d_trajectory_control_init
                        (DTrajectoryControl         *self);

static GObject* d_trajectory_control_constructor
                        (GType                      type,
                         guint                      n_construct_params,
                         GObjectConstructParam      *construct_params);

static void     d_trajectory_control_dispose
                        (GObject                    *obj);

static void     d_trajectory_control_finalize
                        (GObject                    *obj);

static void     d_trajectory_control_notify_timer
                        (int                        signal);

static void     d_trajectory_control_set_current_position
                        (DTrajectoryControl         *self,
                         gsl_vector                    *current_position);

static void     d_trajectory_control_set_current_position_axes
                        (DTrajectoryControl         *self,
                         gsl_vector                    *current_position_axes);

static void     d_trajectory_control_set_current_destination
                        (DTrajectoryControl         *self,
                         gsl_vector                    *current_destination);

static void     d_trajectory_control_set_current_destination_axes
                        (DTrajectoryControl         *self,
                         gsl_vector                    *current_destination_axes);

static gpointer d_trajectory_control_main_loop
                        (gpointer                   *trajectory_control);

static DTrajectory* d_trajectory_control_prepare_trajectory
                        (DTrajectoryControl         *self,
                         gsl_vector                    *destination,
                         DCommandType               type);

static void     d_trajectory_control_execute_trajectory
                        (DTrajectoryControl         *self,
                         DTrajectory               *traj,
                         DCommandType               order_type);

static void     d_trajectory_control_default_output
                        (gsl_vector                    *position,
                         gpointer                   output_data);

/* Implementation */
G_DEFINE_TYPE(DTrajectoryControl, d_trajectory_control, G_TYPE_OBJECT);

static GObject*
d_trajectory_control_constructor (GType                 type,
                                  guint                 n_construct_params,
                                  GObjectConstructParam *construct_params)
{
    static GObject *self = NULL;

    if (!self) {
        self = G_OBJECT_CLASS(d_trajectory_control_parent_class)
                ->constructor(type, n_construct_params, construct_params);
        g_object_add_weak_pointer(self, (gpointer) &self);
        return self;
    }
    return g_object_ref(self);
}

static void
d_trajectory_control_class_init (DTrajectoryControlClass    *klass)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(klass);

    obj_class->constructor = d_trajectory_control_constructor;
    obj_class->dispose = d_trajectory_control_dispose;
    obj_class->finalize = d_trajectory_control_finalize;
}

static void
d_trajectory_control_init (DTrajectoryControl   *self)
{
    self->exit_flag = FALSE;
    self->main_loop = NULL;

    self->orders = g_async_queue_new();

    self->geometry = d_geometry_new(30.0, 50.0, 25.0, 10.0);
    self->current_position_axes = gsl_vector_calloc(3);
    self->current_destination_axes = gsl_vector_calloc(3);
    self->current_position = gsl_vector_calloc(3);
    self->current_destination = gsl_vector_calloc(3);

    self->joint_speed = gsl_vector_calloc(3);
    gsl_vector_set_all(self->joint_speed, G_PI/4.0);
    self->linear_speed = gsl_vector_calloc(3);
    gsl_vector_set_all(self->linear_speed, 20.0);

    self->linear_out_fun = d_trajectory_control_default_output;
    self->linear_out_data = NULL;
    self->joint_out_fun = d_trajectory_control_default_output;
    self->joint_out_data = NULL;

    self->accelTime = 0.1;
    self->decelTime = 0.1;
    self->stepTime = 0.05;
}

static void
d_trajectory_control_dispose (GObject   *obj)
{
    DTrajectoryControl *self = D_TRAJECTORY_CONTROL(obj);

    if (self->main_loop) {
        d_trajectory_control_stop(self);
    }
    if (self->orders) {
        g_async_queue_unref(self->orders);
        self->orders = NULL;
    }
    if (self->geometry) {
        g_object_unref(self->geometry);
        self->geometry = NULL;
    }
    if (self->joint_speed) {
        gsl_vector_free(self->joint_speed);
        self->joint_speed = NULL;
    }
    if (self->linear_speed) {
        gsl_vector_free(self->linear_speed);
        self->linear_speed = NULL;
    }
    if (self->current_position) {
        gsl_vector_free(self->current_position);
        self->current_position = NULL;
    }
    if (self->current_destination) {
        gsl_vector_free(self->current_destination);
        self->current_destination = NULL;
    }
    if (self->current_position_axes) {
        gsl_vector_free(self->current_position_axes);
        self->current_position_axes = NULL;
    }
    if (self->current_destination_axes) {
        gsl_vector_free(self->current_destination_axes);
        self->current_destination_axes = NULL;
    }
    /* Chain Up */
    G_OBJECT_CLASS(d_trajectory_control_parent_class)->dispose(obj);
}

static void
d_trajectory_control_finalize (GObject  *obj)
{
    /* Chain Up */
    G_OBJECT_CLASS(d_trajectory_control_parent_class)->finalize(obj);
}

static void
d_trajectory_control_notify_timer (int signal)
{
    g_cond_signal(&wakeup_cond);
}

static gpointer
d_trajectory_control_main_loop (gpointer    *trajectory_control)
{
    g_return_val_if_fail(D_IS_TRAJECTORY_CONTROL(trajectory_control), NULL);
    DTrajectoryControl *self = D_TRAJECTORY_CONTROL(trajectory_control);

    /* Start waiting for orders */
    while(!self->exit_flag) {
        /* Wait until order is available with a 2 second timeout */
        DTrajectoryCommand *order = NULL;
        while(!order) {
            order = g_async_queue_timeout_pop(self->orders, 2.0 * G_TIME_SPAN_SECOND);
        }
        /* Process the order */
        DTrajectory *trajectory;
        DCommandType type = order->command_type;
        switch (type) {
            case OT_MOVEJ:
                {
                    gsl_vector *destination = (gsl_vector*)(order->data);
                    trajectory = d_trajectory_control_prepare_trajectory(self,
                                                    destination,
                                                    type);
                    d_trajectory_control_set_current_destination_axes (self,
                                                    destination);
                    d_trajectory_control_execute_trajectory(self,
                                                    trajectory,
                                                    type);
                    g_object_unref(trajectory);
                }
                break;
            case OT_MOVEL:
                {
                    gsl_vector *destination = (gsl_vector*)(order->data);
                    trajectory = d_trajectory_control_prepare_trajectory(self,
                                                    destination,
                                                    type);
                    d_trajectory_control_set_current_destination (self,
                                                    destination);
                    d_trajectory_control_execute_trajectory(self,
                                                    trajectory,
                                                    type);
                    g_object_unref(trajectory);
                }
                break;
            case OT_WAIT:
                g_warning("d_trajectory_control_main_loop: no OT_WAIT command, implement!");
                break;
            case OT_END:
                self->exit_flag = TRUE;
                break;
            default:
                g_error("Unknown command type: %i", order->command_type);
        }
        if (order) {
            g_object_unref(order);
            order = NULL;
        }
    }
    g_thread_exit(NULL);
}

static void
d_trajectory_control_execute_wait (DTrajectoryControl   *self,
                                   guint64              *msecs)
{
    g_warning("d_trajectory_control_execute_wait is a stub");
}

static DTrajectory*
d_trajectory_control_prepare_trajectory (DTrajectoryControl     *self,
                                         gsl_vector             *destination,
                                         DCommandType           type)
{
    g_message("d_trajectory_control_prepare_trajectory: Preparing trajectory");
    DTrajectory *traj;

    switch (type) {
        case OT_MOVEJ:
            traj = D_TRAJECTORY(d_joint_trajectory_new_full(
                                    self->current_position_axes,
                                    self->current_destination_axes,
                                    destination,
                                    self->joint_speed,
                                    self->accelTime,
                                    self->stepTime));
            break;
        case OT_MOVEL:
            traj = D_TRAJECTORY(d_linear_trajectory_new_full(
                                    self->current_position,
                                    self->current_destination,
                                    destination,
                                    self->linear_speed,
                                    self->accelTime,
                                    self->stepTime));
            break;
        default:
            g_error("d_trajectory_control_prepare_trajectory: Not yet implemented");
    }
    return traj;
}

static void
d_trajectory_control_execute_trajectory (DTrajectoryControl *self,
                                         DTrajectory        *traj,
                                         DCommandType       order_type)
{
    timer_t timerid;
    struct itimerspec value;
    struct sigevent event;

    //TODO: Allow stepTime greater than 1
    value.it_value.tv_sec = 0;
    value.it_value.tv_nsec = self->stepTime * D_NANOS_PER_SEC;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = self->stepTime * D_NANOS_PER_SEC;

    //TODO: Set the attributes for real-time notification
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_attributes = NULL;
    event.sigev_notify_function = (void*)d_trajectory_control_notify_timer;

    timer_create(CLOCK_REALTIME, &event, &timerid);
    timer_settime(timerid, 0, &value, NULL);

    while(d_trajectory_has_next(traj)) {
            g_mutex_lock(&timer_mutex);
            g_cond_wait(&wakeup_cond, &timer_mutex);
            g_mutex_unlock(&timer_mutex);
            //TODO: Do cleanup
            if (self->exit_flag) {
                g_message("Exiting...\n");
                timer_delete(timerid);
                return;
            }
            switch (order_type) {
                case OT_MOVEJ:
                    d_trajectory_control_set_current_position_axes(self, d_trajectory_next(traj));
                    break;
                case OT_MOVEL:
                    d_trajectory_control_set_current_position(self, d_trajectory_next(traj));
                    break;
                default:
                    g_error("d_trajectory_control_execute_trajectory: unknown trajectory type!");
            }
    }
    g_message("d_trajectory_control_execute_trajectory: Trajectory ended");
    timer_delete(timerid);
}

static void
d_trajectory_control_set_current_destination (DTrajectoryControl    *self,
                                              gsl_vector               *dest)
{
    gsl_vector_memcpy(self->current_destination, dest);
    d_solver_solve_inverse(self->geometry,
                           dest,
                           self->current_destination_axes,
                           NULL);
}

static void
d_trajectory_control_set_current_destination_axes (DTrajectoryControl   *self,
                                                   gsl_vector              *dest_axes)
{
    gsl_vector_memcpy(self->current_destination_axes, dest_axes);
    d_solver_solve_direct(self->geometry,
                          dest_axes,
                          self->current_destination);
}

static void
d_trajectory_control_set_current_position (DTrajectoryControl   *self,
                                           gsl_vector              *pos)
{
    /* Call the output function first so we can avoid delays */
    self->linear_out_fun(pos, self->linear_out_data);
    gsl_vector_memcpy(self->current_position, pos);
    d_solver_solve_inverse(self->geometry,
                           self->current_position,
                           self->current_position_axes,
                           NULL);
}

static void
d_trajectory_control_set_current_position_axes (DTrajectoryControl  *self,
                                                gsl_vector          *axes)
{
    /* Call the output function first so we can avoid delays */
    self->joint_out_fun(axes, self->joint_out_data);
    gsl_vector_memcpy(self->current_position_axes, axes);
    d_solver_solve_direct(self->geometry,
                          self->current_position_axes,
                          self->current_position);
}

static void
d_trajectory_control_default_output (gsl_vector    *position,
                                     gpointer   data)
{
    g_print("Current position: %f, %f, %f\n",
                    gsl_vector_get(position, 0),
                    gsl_vector_get(position, 1),
                    gsl_vector_get(position, 2));
}

/* Public API */
DTrajectoryControl*
d_trajectory_control_new (void)
{
    DTrajectoryControl *self;
    gsl_vector *home_axes;

    home_axes = gsl_vector_calloc(3);
    self = g_object_new(D_TYPE_TRAJECTORY_CONTROL, NULL);

    d_trajectory_control_set_current_position_axes(self, home_axes);
    d_trajectory_control_set_current_destination_axes(self, home_axes);

    return self;
}

void
d_trajectory_control_start (DTrajectoryControl  *self)
{
    g_return_if_fail(D_IS_TRAJECTORY_CONTROL(self));

    self->main_loop = g_thread_new("control_main",
                        (GThreadFunc)d_trajectory_control_main_loop, self);
}

void
d_trajectory_control_stop (DTrajectoryControl   *self)
{
    if (self->main_loop) {
        d_trajectory_control_push_order(self, d_trajectory_command_new(OT_END, NULL));
        self->exit_flag = TRUE;
        g_thread_join(self->main_loop);
        self->main_loop = NULL;
    }
}

void
d_trajectory_control_push_order (DTrajectoryControl *self,
                                 DTrajectoryCommand *order)
{
    g_return_if_fail(D_IS_TRAJECTORY_COMMAND(order));

    g_async_queue_push(self->orders, g_object_ref(order));
}

void
d_trajectory_control_set_linear_out_fun (DTrajectoryControl     *self,
                                         DTrajectoryOutputFunc  out_fun,
                                         gpointer               out_data)
{
    g_return_if_fail(D_IS_TRAJECTORY_CONTROL(self));

    self->linear_out_data = out_data;
    self->linear_out_fun = out_fun;
}

void
d_trajectory_control_set_joint_out_fun (DTrajectoryControl      *self,
                                        DTrajectoryOutputFunc   out_fun,
                                        gpointer                out_data)
{
    g_return_if_fail(D_IS_TRAJECTORY_CONTROL(self));

    self->joint_out_data = out_data;
    self->joint_out_fun = out_fun;
}
