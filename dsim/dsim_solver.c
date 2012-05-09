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

#include <stdlib.h>
#include <math.h>
#include <glib.h>
#include "dsim_solver.h"

/* Static Methods */
void
d_solver_solve_direct ( DGeometry*  geometry,
                        DAxes*      axes,
                        DPos*       pos )
{
    //TODO: Checkear que los centros no sean colineales
    //TODO: Use GError for non-reachable positions
    gdouble pb[3][3];
    {
        int i;
        for(i = 0; i < 3; i++) {
            gdouble phi = ((gdouble)i) * G_PI * 2.0 / 3.0;
            pb[i][0] = cos(phi) * (geometry->r + geometry->a* cos(d_axes_get(axes, i)) - geometry->h);
            pb[i][1] = sin(phi) * (geometry->r + geometry->a* cos(d_axes_get(axes, i)) - geometry->h);
            pb[i][2] = geometry->a * sin(d_axes_get(axes, i));
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
                   e[3][0]*e[1][1] - e[3][1]*e[0][0] };
    gdouble k0 = (l[1]/l[0]) * (l[1]/l[0]) +
                (l[3]/l[0]) * (l[3]/l[0]) +
                pb[0][0] * pb[0][0] +
                pb[0][1] * pb[0][1] +
                pb[0][2] * pb[0][2] -
                geometry->b * geometry->b -
                2.0 * pb[0][0] * l[1] / l[0] -
                2.0 * pb[0][1] * l[3] / l[0];
    gdouble k1 = 2.0 * l[1]*l[2]/(l[0]*l[0])
              + 2.0 * l[3]*l[4]/(l[0]*l[0])
              - 2.0 * pb[0][0] * l[2] / l[0]
              - 2.0 * pb[0][1] * l[4] / l[0]
              - 2.0 * pb[0][2];
    gdouble k2 = (l[2]/l[0])*(l[2]/l[0]) + (l[4]/l[0])*(l[4]/l[0]) + 1.0;

    gdouble disc = k1 * k1 - 4.0 * k2 * k0;
    if ( disc < 0.0) {
        g_warning("Unreachable point");
        return;
    }
    d_pos_set(pos, 2, (-k1 + sqrt(disc)) / 2.0 * k2);
    d_pos_set(pos, 1, l[3]/l[0] + l[4]/l[0] * d_pos_get(pos, 2));
    d_pos_set(pos, 0, l[1]/l[0] + l[2]/l[0] * d_pos_get(pos, 2));
}

void
d_solver_solve_inverse ( DGeometry  *geometry,
                         DPos       *pos,
                         DAxes      *axes,
                         DExtAxes   *extaxes )
{
    gboolean extcalc = extaxes ? TRUE : FALSE;
    {
        int i;
        for (i = 0; i < 3; i++) {
            /* Locate point Ci */
            gdouble phi = ((gdouble) i ) * G_PI * 2.0 / 3.0;
            gdouble ci[] = {
                d_pos_get(pos, 0) * cos(phi) + d_pos_get(pos, 1) * sin(phi) + geometry->h - geometry->r,
                d_pos_get(pos, 1) * cos(phi) - d_pos_get(pos, 0) * sin(phi),
                d_pos_get(pos, 2) };

            /* Calculate theta 3 */
            gdouble cos3 = ci[1] / geometry->b;

            /* Check valid cos3 */
            if ( abs(cos3) >= 1.0 ) {
                g_warning("Unreachable point");
                return;
            } //TODO: Add check for sen3 == 0
            gdouble sen3 = sqrt(1.0 - cos3 * cos3);
            //TODO:Add extended axes calculation
            if (extcalc) {
                d_ext_axes_set(extaxes, i, 2, atan2(sen3, cos3));
            }

            /* Calculate theta 2 */
            gdouble cnormsq = ci[0] * ci[0] + ci[1] * ci[1] + ci[2] * ci[2];
            gdouble cos2 = (cnormsq - geometry->a * geometry->a - geometry->b * geometry->b) / (2.0 * geometry->a * geometry->b * sen3);
            /* Check for valid cos2 */
            if ( abs(cos2) >= 1.0 ) {
                g_warning("Unreachable point");
                return;
            }
            gdouble sen2 = sqrt(1.0 - cos2 * cos2);
            if ( extcalc ) {
                d_ext_axes_set(extaxes, i, 1, atan2(sen2, cos2));
            }

            /* Calculate theta 1 */
            gdouble x1 = geometry->a + geometry->b * cos2 * sen3;
            gdouble x2 = geometry->b * sen2 * sen3;
            gdouble sen1 = ci[2] * x1 - ci[0] * x2;
            gdouble cos1 = ci[2] * x2 + ci[0] * x1;
            if ( extcalc ) {
                d_ext_axes_set(extaxes, i, 0, atan2(sen1, cos1));
            }
            d_axes_set(axes, i, atan2(sen1, cos1));
        }
    }
}



/* GType Register */
G_DEFINE_TYPE (DSolver, d_solver, G_TYPE_OBJECT);

/* Constructor */
DSolver*
d_solver_get_instance (void)
{
    return D_SOLVER(g_object_new(D_TYPE_SOLVER, NULL));
}

static GObject*
constructor ( GType type,
              guint n_construct_params,
              GObjectConstructParam *construct_params )
{
    static GObject *self = NULL;

    if (self == NULL) {
        self = G_OBJECT_CLASS (d_solver_parent_class)->constructor(
                    type, n_construct_params, construct_params);
        g_object_add_weak_pointer (self, (gpointer) &self);
        return (self);
    }

    return g_object_ref (self);
}

/* Destructor */
static void
d_solver_dispose ( GObject *object )
{
    G_OBJECT_CLASS (d_solver_parent_class)->dispose(object);
}

static void
d_solver_finalize ( GObject *object )
{
    G_OBJECT_CLASS (d_solver_parent_class)->finalize(object);
}

/* Init class and instance */
static void
d_solver_class_init ( DSolverClass *klass )
{
    //TODO: Initialize destructors
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->constructor = constructor;
    object_class->dispose = d_solver_dispose;
    object_class->finalize = d_solver_finalize;
}

static void
d_solver_init ( DSolver *self )
{
    //TODO: Initialize members?
}

