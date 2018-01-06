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
 * dsim_solver.h : Solver for the Delta Simulator library.
 *                 A group of static methods for solving direct
 *                 and inverse kinematic problem.
 */

#ifndef  DSIM_SOLVER_INC
#define  DSIM_SOLVER_INC

#include <glib-object.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <dsim/dsim_geometry.h>
#include <math.h>

/* Static Methods */
void        d_solver_solve_direct       (DGeometry          *geometry,
                                         gsl_vector         *axes,
                                         gsl_vector         *pos,
                                         GError             **err);

void        d_solver_solve_direct_with_ext_axes (DGeometry  *geometry,
                                         gsl_matrix         *extaxes,
                                         gsl_vector         *pos,
                                         GError             **err);

void        d_solver_solve_inverse      (DGeometry          *geometry,
                                         gsl_vector         *pos,
                                         gsl_vector         *axes,
                                         gsl_matrix         *extaxes,
                                         GError             **err);

/* Error type for non reachable points */
#define D_SOLVER_ERROR d_solver_error_quark ()

typedef enum {
    D_SOLVER_ERROR_FAILED
} DSolverError;

GQuark      d_solver_error_quark        (void);

#endif   /* ----- #ifndef DSIM_SOLVER_INC  ----- */
