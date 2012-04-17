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

#include <gsl/gsl_matrix_double.h>
#include <gsl/gsl_blas.h>
#include "dsim_calc.h"

int
interpolate_joint ( const DAxes *current,
                    const DAxes *curdest,
                    const DAxes *dest,
                    const double tacc,
                    const double speedData ) {
    dsim_warning("Joint movement not yet implemented!");
    return 1;
}

int
interpolate_cartesian ( const DPos *current,
                        const DPos *curdest,
                        const DPos *dest,
                        const double tacc,
                        const double SpeedData ) {
    dsim_warning("Linear movement not yet implemented!");
    return 1;
}

/**
 * Solves the direct kinematic problem given the DGeometry and DAxes objects.
 * The result is saved on a DPos object.
 *
 * Returns 0 on success, 1 otherwise.
 */
int
solve_direct ( const DGeometry *geometry,
               const DAxes *axes,
               DPos *pos )
{
    //TODO: Checkear que Bi no sean colineales
    const double a = geometry->a;
    const double b = geometry->b;
    const double h = geometry->h;
    const double r = geometry->r;
    const double t[] = { axes->rax[0],
                         axes->rax[1],
                         axes->rax[2] };
    double pb[3][3];
    {
        int i;
        for(i = 0; i < 3; i++) {
            double phi = ((double)i) * M_PI * 2.0 / 3.0;
            pb[i][0] = cos(phi) * (r + a* cos(axes->rax[i]) - h);
            pb[i][1] = sin(phi) * (r + a* cos(axes->rax[i]) - h);
            pb[i][2] = a * sin(axes->rax[i]);
        }
    }
    double e[4][2];
    {
        int j;
        for (j = 1; j < 3; j++) {
            e[0][j-1] = pb[0][0] * pb[0][0] - pb[j][0] * pb[j][0] +
                        pb[0][1] * pb[0][1] - pb[j][1] * pb[j][1] +
                        pb[0][2] * pb[0][2] - pb[j][2] * pb[j][2];
            e[1][j-1] = 2.0*(pb[j][0] - pb[0][0]);
            e[2][j-1] = 2.0*(pb[j][1] - pb[0][1]);
            e[3][j-1] = 2.0*(pb[j][2] - pb[0][2]);
        }
    }
    double l[] = { e[1][0]*e[2][1] - e[1][1]*e[2][0],
                   e[0][1]*e[2][0] - e[0][0]*e[2][1],
                   e[3][1]*e[2][0] - e[3][0]*e[2][1],
                   e[0][0]*e[1][1] - e[0][1]*e[1][0],
                   e[3][0]*e[1][1] - e[3][1]*e[0][0] };
    double k0 = (l[1]/l[0]) * (l[1]/l[0]) +
                (l[3]/l[0]) * (l[3]/l[0]) +
                pb[0][0] * pb[0][0] +
                pb[0][1] * pb[0][1] +
                pb[0][2] * pb[0][2] -
                b * b -
                2.0 * pb[0][0] * l[1] / l[0] -
                2.0 * pb[0][1] * l[3] / l[0];
    double k1 = 2.0 * l[1]*l[2]/(l[0]*l[0])
              + 2.0 * l[3]*l[4]/(l[0]*l[0])
              - 2.0 * pb[0][0] * l[2] / l[0]
              - 2.0 * pb[0][1] * l[4] / l[0]
              - 2.0 * pb[0][2];
    double k2 = (l[2]/l[0])*(l[2]/l[0]) + (l[4]/l[0])*(l[4]/l[0]) + 1.0;

    double disc = k1 * k1 - 4.0 * k2 * k0;
    printf("k: %f, %f, %f\n", k0, k1, k2);
    if ( disc < 0.0) {
        dsim_warning("Unreachable point");
        return 1;
    }
    pos->z = (-k1 + sqrt(disc)) / 2.0 * k2;
    pos->y = l[3]/l[0] + l[4]/l[0] * pos->z;
    pos->x = l[1]/l[0] + l[2]/l[0] * pos->z;
    return 0;
}

/**
 * Solve inverse problem given the DPos data on DGeometry. Optionally
 * compute the extended axes of each limb for drawing purposes.
 *
 * If the point is reachable this function returns 0, if is not it returns 1
 */
int
solve_inverse ( const DGeometry* geometry,
                const DPos* pos,
                DAxes* axes,
                DExtendedAxes* extaxes)
{
    int extcalc = ((extaxes != NULL) ? 1 : 0);
    double p[] = { pos->x,
                  pos->y,
                  pos->z };
    {
        int i;
        for(i = 0; i < 3; i++) {
            /* Locate point Ci */
            double phi = ((double) i ) * M_PI * 2.0 / 3.0;
            double ci[] = {
                p[0] * cos(phi) + p[1] * sin(phi) + geometry->h - geometry->r,
                p[1] * cos(phi) - p[0] * sin(phi),
                            p[2]};

            /* Calculate theta 3 */
            double cos3 = ci[1] / geometry->b;

            /* Check valid cos3 */
            if ( abs(cos3) >= 1.0 ) {
                dsim_warning("Unreachable point");
                return 1;
            } //TODO: Add check for sen3 == 0
            double sen3 = sqrt(1.0 - cos3 * cos3);
            if (extcalc) {
                extaxes->limb[i].rax[2] = atan2(sen3, cos3);
            }

            /* Calculate theta 2 */
            double cnormsq = ci[0] * ci[0] + ci[1] * ci[1] + ci[2] * ci[2];
            double cos2 = (cnormsq - geometry->a * geometry->a - geometry->b * geometry->b) / (2.0 * geometry->a * geometry->b * sen3);
            /* Check for valid cos2 */
            if ( abs(cos2) >= 1.0 ) {
                dsim_warning("Unreachable point");
                return 1;
            }
            double sen2 = sqrt(1.0 - cos2 * cos2);
            if ( extcalc ) {
                extaxes->limb[i].rax[1] = atan2(sen2, cos2);
            }

            /* Calculate theta 1 */
            double x1 = geometry->a + geometry->b * cos2 * sen3;
            double x2 = geometry->b * sen2 * sen3;
            double sen1 = ci[2] * x1 - ci[0] * x2;
            double cos1 = ci[2] * x2 + ci[0] * x1;
            if ( extcalc ) {
                extaxes->limb[i].rax[0] = atan2(sen1, cos1);
            }
            axes->rax[i] = atan2(sen1, cos1);
        }
    }
    return 0;
}
