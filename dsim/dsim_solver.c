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
 * dsim_solver.c :
 */

#include "dsim_solver.h"

/* Static Methods */
void
d_solver_solve_direct (DGeometry    *geometry,
                       gsl_vector   *axes,
                       gsl_vector   *pos,
                       GError       **err)
{
    g_return_if_fail(D_IS_GEOMETRY(geometry));
    g_return_if_fail(axes != NULL);
    g_return_if_fail(pos != NULL);
    g_return_if_fail(err == NULL || *err == NULL);

    //TODO: Checkear que los centros no sean colineales
    //TODO: Add restrictions to axes

    gdouble pb[3][3];
    {
        int i;
        for(i = 0; i < 3; i++) {
            gdouble phi = ((gdouble)i) * G_PI * 2.0 / 3.0;
            gdouble bx = geometry->r + geometry->a * cos(gsl_vector_get(axes, i)) - geometry->h;
            gdouble by = 0.0;
            gdouble bz = geometry->a * sin(gsl_vector_get(axes,i));

            pb[i][0] = bx * cos(phi) - by * sin(phi);
            pb[i][1] = by * cos(phi) + bx * sin(phi);
            pb[i][2] = bz;
        }
    }
    gdouble e[4][2];
    {
        gint j;
        for (j = 1; j < 3; j++) {
            e[0][j-1] = pb[0][0] * pb[0][0] - pb[j][0] * pb[j][0] +
                        pb[0][1] * pb[0][1] - pb[j][1] * pb[j][1] +
                        pb[0][2] * pb[0][2] - pb[j][2] * pb[j][2];
            e[1][j-1] = 2.0*(pb[j][0] - pb[0][0]);
            e[2][j-1] = 2.0*(pb[j][1] - pb[0][1]);
            e[3][j-1] = 2.0*(pb[j][2] - pb[0][2]);
        }
    }
    gdouble l[] = { e[1][0]*e[2][1] - e[1][1]*e[2][0],
                   e[0][1]*e[2][0] - e[0][0]*e[2][1],
                   e[3][1]*e[2][0] - e[3][0]*e[2][1],
                   e[0][0]*e[1][1] - e[0][1]*e[1][0],
                   e[3][0]*e[1][1] - e[3][1]*e[1][0] };
    gdouble k0 = pow(l[1]/l[0], 2.0) +
                pow(l[3]/l[0], 2.0) +
                pow(pb[0][0], 2.0) +
                pow(pb[0][1], 2.0) +
                pow(pb[0][2], 2.0) -
                pow(geometry->b, 2.0) -
                2.0 * pb[0][0] * l[1] / l[0] -
                2.0 * pb[0][1] * l[3] / l[0];
    gdouble k1 = 2.0 * l[1]*l[2]/pow(l[0],2.0)
              + 2.0 * l[3]*l[4]/pow(l[0],2.0)
              - 2.0 * pb[0][0] * l[2] / l[0]
              - 2.0 * pb[0][1] * l[4] / l[0]
              - 2.0 * pb[0][2];
    gdouble k2 = pow(l[2]/l[0], 2.0) + pow(l[4]/l[0], 2.0) + 1.0;

    gdouble disc = k1 * k1 - 4.0 * k2 * k0;
    if ( disc < 0.0) {
        g_set_error_literal(err,
                D_SOLVER_ERROR,
                D_SOLVER_ERROR_FAILED,
                "Could not reach point in cartesian space");
        return;
    }
    gsl_vector_set(pos, 2, ((-k1 + sqrt(disc))) / (2.0 * k2));
    gsl_vector_set(pos, 1, l[3]/l[0] + l[4]/l[0] * gsl_vector_get(pos, 2));
    gsl_vector_set(pos, 0, l[1]/l[0] + l[2]/l[0] * gsl_vector_get(pos, 2));

    g_assert(err == NULL || *err == NULL);
    return;
}

void
d_solver_solve_direct_with_ext_axes (DGeometry  *geometry,
                                     gsl_matrix *extaxes,
                                     gsl_vector *pos,
                                     GError     **err)
{
    g_return_if_fail(D_IS_GEOMETRY(geometry));
    g_return_if_fail(extaxes != NULL);
    g_return_if_fail(pos != NULL);
    g_return_if_fail(err == NULL || *err == NULL);

    gdouble a = geometry->a;
    gdouble b = geometry->b;
    gdouble h = geometry->h;
    gdouble r = geometry->r;

    /* Use three positions that should be the same */
    gdouble p[3][3];
    for (int i = 0; i < 3; i++) {
        gdouble phi = ((gdouble) i) * G_PI * 2.0 / 3.0;
        gdouble ci[] = {
            a * cos(gsl_matrix_get(extaxes, i, 0))
                + b*sin(gsl_matrix_get(extaxes, i, 2))
                *cos(gsl_matrix_get(extaxes, i, 0) + gsl_matrix_get(extaxes, i, 1)),
            b * cos(gsl_matrix_get(extaxes, i, 2)),
            a * sin(gsl_matrix_get(extaxes, i, 0))
                + b*sin(gsl_matrix_get(extaxes, i, 2))
                *sin(gsl_matrix_get(extaxes, i, 0) + gsl_matrix_get(extaxes, i, 1)),
        };
        gdouble px[] = {
            (ci[0] - h + r) * cos(phi) - ci[1] * sin(phi),
            (ci[0] - h + r) * sin(phi) + ci[1] * cos(phi),
            ci[2]
        };

        p[i][0] = px[0];
        p[i][1] = px[1];
        p[i][2] = px[2];
    }

    //TODO: Assert the three equal position vectors

    gsl_vector_set(pos, 0, p[0][0]);
    gsl_vector_set(pos, 1, p[0][1]);
    gsl_vector_set(pos, 2, p[0][2]);

    g_assert(err == NULL || *err == NULL);
    return;
}

void
d_solver_solve_inverse (DGeometry   *geometry,
                        gsl_vector  *pos,
                        gsl_vector  *axes,
                        gsl_matrix  *extaxes,
                        GError      **err)
{
    gboolean extcalc = extaxes ? TRUE : FALSE;
    gboolean axescalc = axes ? TRUE : FALSE;
    g_return_if_fail((extcalc || axescalc));
    g_return_if_fail(err == NULL || *err == NULL);

    //TODO: Add hard restrictions to axes
    for (int i = 0; i < 3; i++) {
        /* Locate point Ci */
        gdouble phi = ((gdouble) i ) * G_PI * 2.0 / 3.0;
        gdouble ci[] = {
        gsl_vector_get(pos, 0) * cos(phi) + gsl_vector_get(pos, 1) * sin(phi) + geometry->h - geometry->r,
        gsl_vector_get(pos, 1) * cos(phi) - gsl_vector_get(pos, 0) * sin(phi),
        gsl_vector_get(pos, 2) };

        /* Calculate theta 3 */
        gdouble cos3 = ci[1] / geometry->b;

        /* Check valid cos3 so we don't get an invalid sen3 */
        if ( abs(cos3) >= 1.0 ) {
            g_set_error(err,
                    D_SOLVER_ERROR,
                    D_SOLVER_ERROR_FAILED,
                    "Target [ %f, %f, %f ] is out of working space",
                    gsl_vector_get(pos, 0),
                    gsl_vector_get(pos, 1),
                    gsl_vector_get(pos, 2));
            return;
        }
        gdouble sen3 = sqrt(1.0 - cos3 * cos3);
        if (extcalc) {
            gsl_matrix_set(extaxes, i, 2, atan2(sen3, cos3));
        }

        /* Calculate theta 2 */
        gdouble cnormsq = ci[0] * ci[0] + ci[1] * ci[1] + ci[2] * ci[2];
        gdouble cos2 = (cnormsq - geometry->a * geometry->a - geometry->b * geometry->b) / (2.0 * geometry->a * geometry->b * sen3);
        /* Check for valid cos2 */
        if ( abs(cos2) >= 1.0 ) {
            g_set_error(err,
                    D_SOLVER_ERROR,
                    D_SOLVER_ERROR_FAILED,
                    "Target [ %f, %f, %f ] is out of working space",
                    gsl_vector_get(pos, 0),
                    gsl_vector_get(pos, 1),
                    gsl_vector_get(pos, 2));
            return;
        }
        gdouble sen2 = sqrt(1.0 - cos2 * cos2);
        if ( extcalc ) {
            gsl_matrix_set(extaxes, i, 1, atan2(sen2, cos2));
        }

        /* Calculate theta 1 */
        gdouble x1 = geometry->a + geometry->b * cos2 * sen3;
        gdouble x2 = geometry->b * sen2 * sen3;
        gdouble sen1 = ci[2] * x1 - ci[0] * x2;
        gdouble cos1 = ci[2] * x2 + ci[0] * x1;
        if ( extcalc ) {
            gsl_matrix_set(extaxes, i, 0, atan2(sen1, cos1));
        }
        if (axescalc) {
            gsl_vector_set(axes, i, atan2(sen1, cos1));
        }
    }

    /* No error occured */
    g_assert(err == NULL || *err == NULL);
    return;
}

/* Error handling functions */
GQuark
d_solver_error_quark (void)
{
    return g_quark_from_static_string("d_solver_error_quark");
}
