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


#ifndef  DSIM_CALC_INC
#define  DSIM_CALC_INC

#include <math.h>
#include <dsim/dsim_common.h>
#include <dsim/dsim_error.h>
#include <dsim/dsim_axes.h>
#include <dsim/dsim_pos.h>

typedef struct {
    double a;
    double b;
    double h;
    double r;
} DGeometry;

/**
 * Solves inverse problem of a given Delta manipulator given the target
 * position p. Uses only the outer configuration for the limbs.
 */
int solve_inverse ( const DGeometry* geometry,
                    const DPos* pos,
                    DAxes* axes,
                    DExtendedAxes* extaxes);

int solve_direct ( const DGeometry *geometry,
                   const DAxes *axes,
                   DPos *pos );

#endif   // ----- #ifndef DSIM_CALC_INC  -----
