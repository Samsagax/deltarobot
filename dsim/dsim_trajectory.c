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

/* #####   DTRAJECTORYINTERFACE IMPLEMENTATION   ###################### */
G_DEFINE_INTERFACE(DITrajectory, d_itrajectory, G_TYPE_OBJECT);

static void
d_itrajectory_default_init(DITrajectoryInterface   *klass)
{
}

DVector*
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

DVector*
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
d_trajectory_interpolate_lspb (DVector  *res_point,
                               DVector  *current_speed,
                               DVector  *segment_speed,
                               DVector  *control_point,
                               gdouble  acceleration_time,
                               gdouble  time)
{

}
