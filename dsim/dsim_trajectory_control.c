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

static gpointer d_trajectory_control_main_loop
                        (gpointer                   *trajectory_control);

static void     d_trajectory_control_worker
                        (DTrajectoryControl         *self);

static void     d_trajectory_control_kill_worker
                        (DTrajectoryControl         *self);

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

    self->point_list = NULL;
    self->current_position = NULL;
    self->destination = NULL;

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
    if (self->point_list) {
        g_object_unref(self->point_list);
        self->point_list = NULL;
    }
    if (self->current_position) {
        g_object_unref(self->current_position);
        self->current_position = NULL;
    }
    if (self->destination) {
        g_object_unref(self->destination);
        self->destination = NULL;
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

    timer_t timerid;
    struct itimerspec value;
    struct sigevent event;

    //TODO: Allow stepTime greater than 1
    value.it_value.tv_sec = 1;
//    value.it_value.tv_nsec = self->stepTime * D_NANOS_PER_SEC;
    value.it_value.tv_nsec = 0;
    value.it_interval.tv_sec = 1;
//    value.it_interval.tv_nsec = self->stepTime * D_NANOS_PER_SEC;
    value.it_interval.tv_nsec = 0;

    //TODO: Set the attributes for real-time notification
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_attributes = NULL;
    event.sigev_notify_function = (void*)d_trajectory_control_notify_timer;

    timer_create(CLOCK_REALTIME, &event, &timerid);
    timer_settime(timerid, 0, &value, NULL);

    /* Start trajectory execution */
    while(!self->exit_flag) {
        g_mutex_lock(&timer_mutex);
        g_cond_wait(&wakeup_cond, &timer_mutex);
        g_mutex_unlock(&timer_mutex);
        if (self->exit_flag) {
            g_print("Exiting...\n");
            timer_delete(timerid);
            g_thread_exit(NULL);
        }
        g_print("Timer Triggered\n");
    }
    timer_delete(timerid);
    g_thread_exit(NULL);
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
    g_warning("d_trajectory_control_start is a stub");
    g_return_if_fail(D_IS_TRAJECTORY_CONTROL(self));

    self->main_loop = g_thread_new("control_main",
                        (GThreadFunc)d_trajectory_control_main_loop, self);
}

void
d_trajectory_control_stop (DTrajectoryControl   *self)
{
    g_warning ("d_trajectory_control_stop is a stub");
    self->exit_flag = TRUE;
    g_cond_signal(&wakeup_cond);
    g_thread_join(self->main_loop);
    self->main_loop = NULL;
}
