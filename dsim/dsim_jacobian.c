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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURJACOBIANE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PROJECTNAME. If not, see <http://www.gnu.org/licenses/>.
 */

#include "dsim_jacobian.h"

/* Static Methods */
void
d_jacobian_direct (gsl_matrix   *direct,
                   DGeometry    *geometry,
                   gsl_matrix   *ext_axes)
{
    for (int i = 0; i < direct->size1; i++) {
        gdouble phi = i * G_PI * 120.0 / 180.0;
        gdouble t[] = {
            gsl_matrix_get(ext_axes, i, 0),
            gsl_matrix_get(ext_axes, i, 1),
            gsl_matrix_get(ext_axes, i, 2)
        };
        gdouble j[] = {
            cos(t[0] + t[1]) * sin(t[2]) * cos(phi) - cos(t[2]) * sin(phi),
            cos(t[0] + t[1]) * sin(t[2]) * sin(phi) + cos(t[2]) * cos(phi),
            sin(t[0] + t[1]) * sin(t[2])
        };
        for (int k = 0; k < direct->size2; k++) {
            gsl_matrix_set(direct, i, k, j[k]);
        }
    }
}

void
d_jacobian_inverse (gsl_matrix   *inverse,
                   DGeometry    *geometry,
                   gsl_matrix   *ext_axes)
{
    for (int i = 0; i < inverse->size1; i++) {
        gdouble t[] = {
            gsl_matrix_get(ext_axes, i, 0),
            gsl_matrix_get(ext_axes, i, 1),
            gsl_matrix_get(ext_axes, i, 2)
        };
        for (int k = 0; k < inverse->size2; k++) {
            if (i == k) {
                gdouble j = sin(t[1]) * sin (t[2]);
                gsl_matrix_set(inverse, i, k, j);
            } else {
                gsl_matrix_set(inverse, i, k, 0.0);
            }
        }
    }
}
