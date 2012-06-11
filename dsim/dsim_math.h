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
 * dsim_math.h : Extra math functions needed across the entire library.
 */

#ifndef  DSIM_MATH_INC
#define  DSIM_MATH_INC

#include <math.h>
#include <glib.h>

gint            d_math_sign                 (gdouble    a);

gint            d_math_signf                (gfloat     a);

gboolean        d_math_equals_within_epsilon(gdouble    a,
                                             gdouble    b,
                                             gdouble    epsilon);

gboolean        d_math_equals_within_epsilonf (gfloat   a,
                                             gfloat     b,
                                             gfloat     epsilon);

gboolean        d_math_equals_within_ulps   (gdouble    a,
                                             gdouble    b,
                                             guint64    max_ulps);

gboolean        d_math_equals_within_ulpsf  (gfloat     a,
                                             gfloat     b,
                                             guint32    max_ulps);

gboolean        d_math_equals               (gdouble    a,
                                             gdouble    b);

gboolean        d_math_equalsf              (gfloat     a,
                                             gfloat     b);

#endif   /* ----- #ifndef DSIM_MATH_INC  ----- */
