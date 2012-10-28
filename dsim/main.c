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

#include "dsim.h"

int
main (int argc, char* argv[]) {

    g_type_init();

    DGeometry *geometry = d_geometry_new(30.0, 40.0, 10.0, 20.0);
    DDynamicSpec *ds = d_dynamic_spec_new();
    DManipulator *manipulator = d_manipulator_new(geometry, ds);
    DDynamicModel *model = d_dynamic_model_new(manipulator);

    DVector *g = d_vector_new(3);
    d_vector_set(g, 2, 10.0);
    DVector *t0 = d_axes_new_full(1.0, 1.0, 1.0);

    d_dynamic_model_set_axes(model, t0);
    d_dynamic_model_set_gravity(model, g);
    gdouble time = 0.0;

    for (int i=0; i < 100; i++) {
        time += 0.01;
        d_dynamic_model_solve_inverse(model, 0.01);
        DVector *t = d_dynamic_model_get_axes(model);
        DVector *t_dot = d_dynamic_model_get_speed(model);
        g_print("t: %2.5f,\n\tq  = [%2.5f,%2.5f,%2.5f]\n\tq. = [%2.5f,%2.5f,%2.5f]\n",
            time,
            d_vector_get(t, 0),
            d_vector_get(t, 1),
            d_vector_get(t, 2),
            d_vector_get(t_dot, 0),
            d_vector_get(t_dot, 1),
            d_vector_get(t_dot, 2));
    }
    g_object_unref(ds);
    g_object_unref(geometry);
    g_object_unref(model);
    g_object_unref(g);
    g_object_unref(t0);
    g_object_unref(manipulator);
    return 0;
}
