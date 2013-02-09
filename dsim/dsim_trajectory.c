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

/* Forward declarations */
static void
d_trajectory_class_init             (DTrajectoryClass   *klass);

static void
d_trajectory_init                   (DTrajectory        *self);

static void
d_trajectory_dispose                (GObject            *obj);

static void
d_trajectory_finalize               (GObject            *obj);

static void
d_trajectory_interpolate_fun        (DTrajectory        *self);

static void
d_trajectory_real_interpolate_fun   (DTrajectory        *self);

static gsl_vector*
d_trajectory_real_get_destination   (DTrajectory        *self);

static gboolean
d_trajectory_real_has_next          (DTrajectory        *self);

static gsl_vector*
d_trajectory_real_next              (DTrajectory        *self);

static gdouble
d_trajectory_real_get_step_time     (DTrajectory        *self);

static void
d_trajectory_interpolate_lspb       (gsl_vector         *res_point,
                                     gsl_vector         *start_speed,
                                     gsl_vector         *end_speed,
                                     gsl_vector         *control_point,
                                     gdouble            acceleration_time,
                                     gdouble            time);

/* Implementation internals */
G_DEFINE_TYPE(DTrajectory, d_trajectory, G_TYPE_OBJECT);

static void
d_trajectory_class_init (DTrajectoryClass    *klass)
{
    GObjectClass *goc = G_OBJECT_CLASS(klass);
    goc->dispose = d_trajectory_dispose;
    goc->finalize = d_trajectory_finalize;

    klass->interpolate_fun = d_trajectory_real_interpolate_fun;
    klass->get_destination = d_trajectory_real_get_destination;
    klass->has_next = d_trajectory_real_has_next;
    klass->next = d_trajectory_real_next;
    klass->get_step_time = d_trajectory_real_get_step_time;
}

static void
d_trajectory_init (DTrajectory   *self)
{
    self->time = 0.0;
    self->move_time = 0.0;
    self->acceleration_time = 0.0;
    self->step_time = 0.0;

    self->current = gsl_vector_calloc(3);
    self->destination = gsl_vector_calloc(3);

    self->start_speed = gsl_vector_calloc(3);
    self->end_speed = gsl_vector_calloc(3);
    self->control_point = gsl_vector_calloc(3);
}

static void
d_trajectory_dispose (GObject    *obj)
{
    DTrajectory *self = D_TRAJECTORY(obj);

    if (self->current) {
        gsl_vector_free(self->current);
        self->current = NULL;
    }
    if (self->destination) {
        gsl_vector_free(self->destination);
        self->destination = NULL;
    }
    if (self->start_speed) {
        gsl_vector_free(self->start_speed);
        self->start_speed = NULL;
    }
    if (self->end_speed) {
        gsl_vector_free(self->end_speed);
        self->end_speed = NULL;
    }
    if (self->control_point) {
        gsl_vector_free(self->control_point);
        self->control_point = NULL;
    }

    /* Chain up */
    G_OBJECT_CLASS(d_trajectory_parent_class)->dispose(obj);
}

static void
d_trajectory_finalize (GObject  *obj)
{
    /* Chain up */
    G_OBJECT_CLASS(d_trajectory_parent_class)->finalize(obj);
}

/* Methods */
static gboolean
d_trajectory_real_has_next (DTrajectory *self)
{
    g_return_val_if_fail(D_IS_TRAJECTORY(self), FALSE);

    DTrajectory *traj = D_TRAJECTORY(self);

    if (traj->time < traj->move_time - traj->acceleration_time) {
        return TRUE;
    }
    return FALSE;
}

static gsl_vector*
d_trajectory_real_get_destination (DTrajectory  *self)
{
    g_return_val_if_fail(D_IS_TRAJECTORY(self), NULL);

    DTrajectory *traj = D_TRAJECTORY(self);

    return traj->destination;
}

static gsl_vector*
d_trajectory_real_next (DTrajectory *self)
{
    g_return_val_if_fail(D_IS_TRAJECTORY(self), NULL);

    self->time += self->step_time;
    d_trajectory_interpolate_fun (self);

    return self->current;
}

static gdouble
d_trajectory_real_get_step_time (DTrajectory    *self)
{
    g_return_val_if_fail(D_IS_JOINT_TRAJECTORY(self), 0.0);

    return self->step_time;
}

static void
d_trajectory_interpolate_fun (DTrajectory   *self)
{
    g_return_if_fail(D_IS_TRAJECTORY(self));
    D_TRAJECTORY_GET_CLASS(self)->interpolate_fun(self);
}

static void
d_trajectory_real_interpolate_fun (DTrajectory  *self)
{
    d_trajectory_interpolate_lspb(self->current,
                                  self->start_speed,
                                  self->end_speed,
                                  self->control_point,
                                  self->acceleration_time,
                                  self->time);
}

static void
d_trajectory_interpolate_lspb (gsl_vector   *res_point,
                               gsl_vector   *start_speed,
                               gsl_vector   *end_speed,
                               gsl_vector   *control_point,
                               gdouble      acceleration_time,
                               gdouble      time)
{
    gdouble fact_end, fact_start;
    if (time > acceleration_time) {
        fact_end = time;
        fact_start = 0.0;
    } else {
        fact_end = pow(time + acceleration_time, 2.0)
                        / (4.0 * acceleration_time);
        fact_start = pow(time - acceleration_time, 2.0)
                        / (4.0 * acceleration_time);
    }
    for (int i = 0; i < 3; i++) {
        gsl_vector_set(res_point, i,
                  gsl_vector_get(control_point, i)
                + gsl_vector_get(end_speed, i) * fact_end
                - gsl_vector_get(start_speed, i) * fact_start);
    }
}

/* Public API */
gsl_vector*
d_trajectory_get_destination (DTrajectory *self)
{
    g_return_val_if_fail(D_IS_TRAJECTORY(self), NULL);
    return D_TRAJECTORY_GET_CLASS(self)->get_destination(self);
}

gboolean
d_trajectory_has_next (DTrajectory *self)
{
    g_return_val_if_fail(D_IS_TRAJECTORY(self), FALSE);
    return D_TRAJECTORY_GET_CLASS(self)->has_next(self);
}

gsl_vector*
d_trajectory_next (DTrajectory     *self)
{
    g_return_val_if_fail (D_IS_TRAJECTORY(self), NULL);
    return D_TRAJECTORY_GET_CLASS(self)->next(self);
}

gdouble
d_trajectory_get_step_time (DTrajectory    *self)
{
    g_return_val_if_fail (D_IS_TRAJECTORY(self), 0.0);
    return D_TRAJECTORY_GET_CLASS(self)->get_step_time(self);
}

gdouble
d_trajectory_calculate_move_time (gsl_vector  *displacement,
                                  gsl_vector  *speed,
                                  gdouble  acceleration_time)
{
    gdouble values[4] = {
        abs(gsl_vector_get(displacement, 0) / gsl_vector_get(speed, 0)),
        abs(gsl_vector_get(displacement, 1) / gsl_vector_get(speed, 1)),
        abs(gsl_vector_get(displacement, 2) / gsl_vector_get(speed, 2)),
        2.0 * acceleration_time
    };
//    g_message("displacement: %f, %f, %f", gsl_vector_get(displacement, 0),
//                        gsl_vector_get(displacement, 1),
//                        gsl_vector_get(displacement, 2));
    gdouble max = values[0];
    {
        int i;
        for(i = 1; i < 4; i++) {
            max = fmax (values[i-1], values[i]);
        }
    }
//    g_message("d_trajectory_calculate_move_time: move time: %f", max);
    return max;
}
