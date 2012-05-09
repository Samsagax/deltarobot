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
 * dsim_solver.h : Singleton Class being the main solver for the
 * Delta Simulator library.
 */

#ifndef  DSIM_SOLVER_INC
#define  DSIM_SOLVER_INC

#include <glib-object.h>
#include <dsim/dsim_pos.h>
#include <dsim/dsim_axes.h>
#include <dsim/dsim_geometry.h>

/* Type macros */
#define D_TYPE_SOLVER               (d_solver_get_type ())
#define D_SOLVER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_SOLVER, DSolver))
#define D_IS_SOLVER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_SOLVER))
#define D_SOLVER_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_SOLVER, DSolverClass))
#define D_IS_SOLVER_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_SOLVER))
#define D_SOLVER_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_SOLVER, DSolverClass))

/* Instance Structure of DSolver */
typedef struct _DSolver DSolver;
struct _DSolver {
    GObject         parent_instance;
};

/* Class Structure of DSolver */
typedef struct _DSolverClass DSolverClass;
struct _DSolverClass {
    GObjectClass    parent_class;
};

/* Returns GType associated with this object type */
GType       d_solver_get_type       (void);

/* Get Solver instance */
DSolver*    d_solver_get_instance   (void);

/* Methods */
/* Static methods */
void d_solver_solve_direct          (DGeometry* geometry,
                                     DAxes*     axes,
                                     DPos*      pos);

void d_solver_solve_inverse         (DGeometry *geometry,
                                     DPos      *pos,
                                     DAxes     *axes,
                                     DExtAxes   *extaxes);


#endif   /* ----- #ifndef DSIM_SOLVER_INC  ----- */
