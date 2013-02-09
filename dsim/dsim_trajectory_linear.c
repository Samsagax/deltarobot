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
d_linear_trajectory_class_init          (DLinearTrajectoryClass *klass);

static void
d_linear_trajectory_init                (DLinearTrajectory      *self);

static void
d_linear_trajectory_dispose             (GObject                *obj);

static void
d_linear_trajectory_finalize            (GObject                *obj);

/* Implementation Internals */
G_DEFINE_TYPE (DLinearTrajectory, d_linear_trajectory, D_TYPE_TRAJECTORY);

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
}

static void
d_linear_trajectory_dispose (GObject    *obj)
{
    /* Chain up */
    G_OBJECT_CLASS(d_linear_trajectory_parent_class)->dispose(obj);
}

static void
d_linear_trajectory_finalize (GObject   *obj)
{
    /* Chain up */
    G_OBJECT_CLASS(d_linear_trajectory_parent_class)->finalize(obj);
}

/* Public API */
DLinearTrajectory*
d_linear_trajectory_new (gsl_vector    *current_pos,
                         gsl_vector    *control_point,
                         gsl_vector    *move_destination,
                         gsl_vector    *speed)
{
    return d_linear_trajectory_new_full (current_pos,
                                         control_point,
                                         move_destination,
                                         speed,
                                         D_DEFAULT_ACC_TIME,
                                         D_DEFAULT_STEP_TIME );
}

DLinearTrajectory*
d_linear_trajectory_new_full (gsl_vector   *current_pos,
                              gsl_vector   *control_point,
                              gsl_vector   *move_destination,
                              gsl_vector   *speed,
                              gdouble   acceleration_time,
                              gdouble   step_time)
{
//    g_warning("d_linear_trajectory_new_full is a stub!");

    DLinearTrajectory* self;
    DTrajectory *parent;
    self = g_object_new (D_TYPE_LINEAR_TRAJECTORY, NULL);
    parent = D_TRAJECTORY(self);

    parent->time = -acceleration_time;
    parent->acceleration_time = acceleration_time;

    gsl_vector_memcpy(parent->current, current_pos);
    gsl_vector_memcpy(parent->destination, move_destination);

    gsl_vector_memcpy(parent->start_speed, control_point);
    gsl_vector_sub(parent->start_speed, current_pos);
    gsl_vector_scale(parent->start_speed, 1.0 / acceleration_time);

    gsl_vector *displacement = gsl_vector_alloc(3);
    gsl_vector_memcpy(displacement, move_destination);
    gsl_vector_sub(displacement, control_point);

    parent->move_time = d_trajectory_calculate_move_time(displacement,
                                                         speed,
                                                         acceleration_time);

    gsl_vector_scale(displacement, parent->move_time);
    gsl_vector_memcpy(parent->end_speed, displacement);

    gsl_vector_memcpy(parent->control_point, control_point);

    parent->step_time = step_time;

    gsl_vector_free(displacement);

    return self;
}

