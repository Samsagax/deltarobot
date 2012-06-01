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

#include <stdio.h>
#include "dsim.h"

int
main (int argc, char* argv[]) {

    g_type_init();

    GString     *string;
    DGeometry   *geometry;
    DVector3    *ax_from;
    DVector3    *ax_to;
    DVector3    *speed;

    string = g_string_new(NULL);
    geometry = d_geometry_new(
                40.0,               /* Lower Limb */
                50.0,               /* Upper Limb */
                30.0,               /* Fixed Platform */
                20.0                /* Moving Platform */
                );
    ax_from = d_axes_new_full(0.0, 0.0, 0.0);
    ax_to = d_axes_new_full(0.0, 0.0, 3.0);
    speed = d_speed_new_full(90.0, 90.0, 90.0);

    d_vector3_to_string(ax_from, string);
    g_print("FROM : %s\n", string->str);
    d_vector3_to_string(ax_to, string);
    g_print("TO:    %s\n", string->str);
    d_vector3_to_string(speed, string);
    g_print("SPEED: %s\n", string->str);

    DJointTrajectory *movej = d_joint_trajectory_new(D_AXES(ax_from),
                                                     D_AXES(ax_from),
                                                     D_AXES(ax_to),
                                                     D_SPEED(speed));
    for (int i = 0; i < 10; i++) {
        DAxes* current = D_AXES(d_trajectory_next(D_ITRAJECTORY(movej)));
        d_vector3_to_string(D_VECTOR3(current), string);
        g_print("AT:    %s\n", string->str);
        g_object_unref(current);
    }

    g_object_unref(movej);
    g_object_unref(speed);
    g_object_unref(geometry);
    g_object_unref(ax_from);
    g_object_unref(ax_to);
    g_string_free(string, TRUE);
    return 0;
}
