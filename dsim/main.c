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
#include "dsim_solver.h"
#include "dsim_axes.h"
#include "dsim_pos.h"
#include "dsim_geometry.h"
#include "dsim_speed.h"

int
main (int argc, char* argv[]) {
    g_type_init();
    GString     *string;
    DGeometry   *geometry;
    DPos        *pos;
    DAxes       *axes;
    DExtAxes    *extaxes;
    DSpeed      *speed;
    string = g_string_new(NULL);
    geometry = d_geometry_new(
                40.0,               /* Lower Limb */
                50.0,               /* Upper Limb */
                30.0,               /* Fixed Platform */
                20.0                /* Moving Platform */
                );
    pos = d_pos_new_full(4.0, 32.0, 36.0);
    axes = d_axes_new();
    extaxes = d_ext_axes_new();


    d_pos_to_string(pos, string);
    printf("DPOS : %s\n", string->str);
    d_axes_to_string(axes, string);
    printf("DAXES: %s\n", string->str);

    d_solver_solve_inverse(geometry, pos, axes, NULL);

    d_pos_to_string(pos, string);
    printf("DPOS : %s\n", string->str);
    d_axes_to_string(axes, string);
    printf("DAXES: %s\n", string->str);


    d_solver_solve_direct(geometry, axes, pos);

    d_pos_to_string(pos, string);
    printf("DPOS : %s\n", string->str);
    d_axes_to_string(axes, string);
    printf("DAXES: %s\n", string->str);

    speed = d_speed_new(90.0, 90.0, 90.0);
    d_speed_to_string(speed, string);
    g_print("SPEED: %s\n", string->str);

    g_object_unref(speed);
    g_object_unref(geometry);
    g_object_unref(pos);
    g_object_unref(axes);
    g_object_unref(extaxes);
    g_string_free(string, TRUE);
    return 0;
}
