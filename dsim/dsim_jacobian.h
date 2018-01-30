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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURDIRECT_JACOBIANE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PROJECTNAME. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * dsim_jacobian.h: Objects representing direct/inverse jacobian of the manipulator
 */

#ifndef  DSIM_JACOBIAN_INC
#define  DSIM_JACOBIAN_INC

#include <math.h>
#include <glib.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <dsim_geometry.h>

/* Static Methods */
void    d_jacobian_direct (gsl_matrix   *direct,
                           DGeometry    *geometry,
                           gsl_matrix   *ext_axes);

void    d_jacobian_inverse (gsl_matrix  *inverse,
                            DGeometry   *geometry,
                            gsl_matrix  *ext_axes);

void    d_jacobian_conventional (gsl_matrix     *jacobian,
                                 DGeometry      *geometry,
                                 gsl_matrix     *ext_axes);

gdouble d_jacobian_dexterity (DGeometry     *geometry,
                              gsl_matrix    *ext_axes);

#endif   /* ----- #ifndef DSIM_JACOBIAN_INC  ----- */

