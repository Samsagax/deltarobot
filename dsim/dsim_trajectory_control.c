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

#include <signal.h>
#include <time.h>

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
                         DVector                    *current_position);

static void     d_trajectory_control_set_current_destination
                        (DTrajectoryControl         *self,
                         DVector                    *current_destination);

static gpointer d_trajectory_control_main_loop
                        (gpointer                   *trajectory_control);

static void     d_trajectory_control_execute_trajectory
                        (DTrajectoryControl         *self,
                         DITrajectory               *traj);

static void     d_trajectory_control_default_output
                        (DVector                    *position,
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
    g_warning("d_trajectory_control_init is a stub");

    self->exit_flag = FALSE;
    self->main_loop = NULL;

    self->orders = g_async_queue_new();
    self->current_position = NULL;
    self->current_destination = NULL;
    self->output_func = d_trajectory_control_default_output;
    self->output_data = NULL;

    self->accelTime = 0.1;
    self->decelTime = 0.1;
    self->stepTime = 0.01;
}

static void
d_trajectory_control_dispose (GObject   *obj)
{
    g_warning("d_trajectory_control_dispose is a stub");
    DTrajectoryControl *self = D_TRAJECTORY_CONTROL(obj);

    if (self->main_loop) {
        d_trajectory_control_stop(self);
    }
    if (self->orders) {
        g_async_queue_unref(self->orders);
        self->orders = NULL;
    }
    if (self->current_position) {
        g_object_unref(self->current_position);
        self->current_position = NULL;
    }
    if (self->current_destination) {
        g_object_unref(self->current_destination);
        self->current_destination = NULL;
    }
    /* Chain Up */
    G_OBJECT_CLASS(d_trajectory_control_parent_class)->dispose(obj);
}

static void
d_trajectory_control_finalize (GObject  *obj)
{
    g_warning("d_trajectory_control_finalize is a stub");
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

    /* Hardcoded Dummy trajectory */
    DVector *ax_from = d_axes_new_full(0.0, 0.0, 0.0);
    DVector *ax_to = d_axes_new_full(0.0, 0.0, 6.0);
    DVector *speed = d_speed_new_full(3.0, 3.0, 3.0);
    DJointTrajectory *djt = d_joint_trajectory_new_full
                                        (ax_from,
                                         ax_from,
                                         ax_to,
                                         speed,
                                         self->accelTime,
                                         self->stepTime);
    DTrajectoryCommand *dummy = d_trajectory_command_new(OT_MOVEJ, djt);
    d_trajectory_control_push_order(self, dummy);

    /* Start waiting for orders */
    while(!self->exit_flag) {
        /* Wait until order is available with a 2 second timeout */
        DTrajectoryCommand *order = NULL;
        while(!order) {
            order = g_async_queue_timeout_pop(self->orders, 2.0 * G_TIME_SPAN_SECOND);
        }
        /* Process the order */
        switch (order->command_type) {
            case OT_MOVEJ:
                d_trajectory_control_execute_trajectory(self, D_ITRAJECTORY(order->data));
                break;
            case OT_MOVEL:
                g_warning("d_trajectory_control_main_loop: no OT_MOVEL command, implement!");
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

static void
d_trajectory_control_execute_trajectory (DTrajectoryControl *self,
                                         DITrajectory       *traj)
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
            DVector* axes = d_trajectory_next(traj);
            self->output_func(axes, self->output_data);
    }
    timer_delete(timerid);
}

static void
d_trajectory_control_set_current_destination (DTrajectoryControl    *self,
                                              DVector               *dest)
{
    if (self->current_destination) {
        g_object_unref(self->current_destination);
    }
    self->current_destination = g_object_ref(dest);
}

static void
d_trajectory_control_set_current_position (DTrajectoryControl   *self,
                                           DVector              *pos)
{
    /* Call the output function first so we can avoid delays */
    self->output_func(pos, self->output_data);
    if (self->current_position) {
        g_object_unref(self->current_position);
    }
    self->current_position = g_object_ref(pos);
}

static void
d_trajectory_control_default_output (DVector    *position,
                                     gpointer   data)
{
    g_print("Current position: %f, %f, %f\n",
                    d_vector_get(position, 0),
                    d_vector_get(position, 1),
                    d_vector_get(position, 2));
}

/* Public API */
DTrajectoryControl*
d_trajectory_control_new (void)
{
    DTrajectoryControl *dtc;
    dtc = g_object_new(D_TYPE_TRAJECTORY_CONTROL, NULL);
    return dtc;
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
    d_trajectory_control_push_order(self, d_trajectory_command_new(OT_END, NULL));
    self->exit_flag = TRUE;
    g_thread_join(self->main_loop);
    self->main_loop = NULL;
}

void
d_trajectory_control_push_order (DTrajectoryControl *self,
                                 DTrajectoryCommand *order)
{
    g_return_if_fail(D_IS_TRAJECTORY_COMMAND(order));

    g_async_queue_push(self->orders, order);
}

void
d_trajectory_control_set_output_func (DTrajectoryControl    *self,
                                      DTrajectoryOutputFunc func,
                                      gpointer              output_data)
{
    g_return_if_fail(D_IS_TRAJECTORY_CONTROL(self));

    self->output_data = output_data;
    self->output_func = func;
}
