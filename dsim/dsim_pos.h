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

#ifndef DPOS_H
#define DPOS_H

#include <gsl/gsl_vector_float.h>

typedef struct {
    double x, y, z;
} DPos;

/**
 * Returns a pointer to a vector of length 3 out of the given pos structure.
 * The vector memmory needs to be freed afterwards by calling gsl_vector_free.
 */
int dpos_tovectorf( const DPos* p, gsl_vector_float* v );

#endif //DPOS_H
