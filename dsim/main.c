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

//#include <unistd.h>
//#include <stdio.h>
#include <signal.h>
#include <time.h>

#include "dsim.h"

#define NANOS_PER_SEC 1000000000;

GMutex tic_mutex;
GCond tic_cond;
GMutex end_mutex;
GCond end_cond;

static gchar*
d_vector_to_string (DVector *v,
                    GString *s)
{
    g_string_printf(s,
                    "[ %f ; %f ; %f ]",
                    d_vector_get(v, 0),
                    d_vector_get(v, 1),
                    d_vector_get(v, 2));
    return s->str;
}
static void
print_time(int sig)
{
    printf("Timer triggered\n");
    g_cond_signal(&tic_cond);
}

static void
timed_trajectory (gpointer  data)
{
    g_return_if_fail(D_IS_TRAJECTORY(data));

    DTrajectory *traj = D_TRAJECTORY(data);

    timer_t timerid;
    struct itimerspec value;
    struct sigevent event;

    value.it_value.tv_sec = 0;
    value.it_value.tv_nsec = d_trajectory_get_step_time(traj) * NANOS_PER_SEC;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = d_trajectory_get_step_time(traj) * NANOS_PER_SEC;

    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_attributes = NULL;
//    event.sigev_signo = SIGALRM;
    event.sigev_notify_function = (void*)print_time;

    timer_create(CLOCK_REALTIME, &event, &timerid);
    timer_settime(timerid, 0, &value, NULL);

    GString *string = g_string_new(NULL);

    while (d_trajectory_has_next(traj)) {
        g_mutex_lock(&tic_mutex);
        g_cond_wait(&tic_cond, &tic_mutex);
        g_mutex_unlock(&tic_mutex);
        DVector* current = d_trajectory_next(traj);
        g_print("AT:    %s\n", d_vector_to_string(current, string));
        g_object_unref(current);
    }
//
//    for (int i = 0; i < 10; i++) {
//        g_mutex_lock(&tic_mutex);
//        g_cond_wait(&tic_cond, &tic_mutex);
//        g_print("Doing stuff\n");
//        g_usleep(G_USEC_PER_SEC*0.01);
//        g_mutex_unlock(&tic_mutex);
//        g_print("Stuff done\n");
//    }
    timer_delete(timerid);
    g_thread_exit(NULL);
}

static void
print_point(void *data)
{
    while (1) {
        g_mutex_lock(&end_mutex);
        gint64 end_time = g_get_monotonic_time() + 0.2 * G_TIME_SPAN_SECOND;
        if (g_cond_wait_until(&end_cond, &end_mutex, end_time)) {
            g_mutex_unlock(&end_mutex);
            g_thread_exit(NULL);
        }
        g_print(".");
        g_mutex_unlock(&end_mutex);
    }
}



int
main (int argc, char* argv[]) {

    g_type_init();

    DGeometry *geometry = d_geometry_new(30.0, 40.0, 10.0, 20.0);
    DDynamicSpec *ds = d_dynamic_spec_new();
    DDynamicModel *model = d_dynamic_model_new(geometry, ds);

    d_dynamic_model_solve_inverse_axes(model);

    g_object_unref(ds);
    g_object_unref(geometry);
    g_object_unref(model);
    return 0;
}
