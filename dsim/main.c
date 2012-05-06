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

int
main (int argc, char* argv[]) {
    g_type_init();
    DGeometry   *geometry;
    DPos        *pos;
    DAxes       *axes;
    geometry = d_geometry_new(40.0, 50.0, 30.0, 20.0);
    pos = d_pos_new_full(0.0, 0.0, 50.0);
    axes = d_axes_new();
    
    gchar* str = d_pos_to_string(pos);
    printf("DPOS : %s\n", str);
    g_free(str);
    str = d_axes_to_string(axes);
    printf("DAXES: %s\n", str);
    g_free(str);

    d_solver_solve_inverse(geometry, pos, axes);

    str = d_pos_to_string(pos);
    printf("DPOS : %s\n", str);
    g_free(str);
    str = d_axes_to_string(axes);
    printf("DAXES: %s\n", str);
    g_free(str);

    
    d_solver_solve_direct(geometry, axes, pos);
    
    str = d_pos_to_string(pos);
    printf("DPOS : %s\n", str);
    g_free(str);
    str = d_axes_to_string(axes);
    printf("DAXES: %s\n", str);
    g_free(str);
    
    g_object_unref(geometry);
    g_object_unref(pos);
    g_object_unref(axes);
    return 0;
}
