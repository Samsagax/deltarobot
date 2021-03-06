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
 * dsim_trajectory_interface.c :
 */

#include "dsim_trajectory.h"

/* Implementation internals */
G_DEFINE_INTERFACE(DITrajectory, d_itrajectory, G_TYPE_OBJECT);

static void
d_itrajectory_default_init(DITrajectoryInterface   *klass)
{
}

/* Public API */
gsl_vector*
d_trajectory_get_destination (DITrajectory *self)
{
    g_return_val_if_fail(D_IS_ITRAJECTORY(self), NULL);
    return D_ITRAJECTORY_GET_INTERFACE(self)->get_destination(self);
}

gboolean
d_trajectory_has_next (DITrajectory *self)
{
    g_return_val_if_fail(D_IS_ITRAJECTORY(self), FALSE);
    return D_ITRAJECTORY_GET_INTERFACE(self)->has_next(self);
}

gsl_vector*
d_trajectory_next (DITrajectory     *self)
{
    g_return_val_if_fail (D_IS_ITRAJECTORY(self), NULL);
    return D_ITRAJECTORY_GET_INTERFACE(self)->next(self);
}

gdouble
d_trajectory_get_step_time (DITrajectory    *self)
{
    g_return_val_if_fail (D_IS_ITRAJECTORY(self), 0.0);
    return D_ITRAJECTORY_GET_INTERFACE(self)->get_step_time(self);
}

void
d_trajectory_interpolate_lspb (gsl_vector  *res_point,
                               gsl_vector  *start_speed,
                               gsl_vector  *end_speed,
                               gsl_vector  *control_point,
                               gdouble  acceleration_time,
                               gdouble  time)
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
