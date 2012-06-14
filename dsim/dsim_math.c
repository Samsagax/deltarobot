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
 * dsim_math.c :
 */

#include "dsim_math.h"

#define MAX_ULPS_DEFAULT 10

typedef union _DDoubleRepresentation DDoubleRepresentation;
union _DDoubleRepresentation {
    gdouble value;
    guint64 representation;
};

typedef union _DFloatRepresentation DFloatRepresentation;
union _DFloatRepresentation {
    gfloat  value;
    guint32 representation;
};

gint
d_math_sign (gdouble a)
{
    if (signbit(a)) {
        return -1;
    }
    return 1;
}

gint
d_math_signf (gfloat a)
{
    if (signbit(a)) {
        return -1;
    }
    return 1;
}

static gint64
d_math_ulp_distance (gdouble a,
                     gdouble b)
{
    DDoubleRepresentation a_rep = { a };
    DDoubleRepresentation b_rep = { b };
    gint64 ulp_distance;

    ulp_distance = a_rep.representation - b_rep.representation;

    return ulp_distance;
}

static gint32
d_math_ulp_distancef (gfloat a,
                      gfloat b)
{
    DFloatRepresentation a_rep = { a };
    DFloatRepresentation b_rep = { b };
    gint32 ulp_distance;

    ulp_distance = a_rep.representation - b_rep.representation;

    return ulp_distance;
}

gboolean
d_math_equals_within_ulps (gdouble  a,
                           gdouble  b,
                           guint64  max_ulps)
{
    if (d_math_sign(a) != d_math_sign(b)) {
        if (a == b) {
            return TRUE;
        }
        return FALSE;
    }
    guint64 ulps = ABS (d_math_ulp_distance(a, b));
    if (ulps <= max_ulps) {
        return TRUE;
    }
    return FALSE;
}

gboolean
d_math_equals_within_ulpsf (gfloat  a,
                            gfloat  b,
                            guint32 max_ulps)
{
    if (d_math_sign(a) != d_math_sign(b)) {
        if (a == b) {
            return TRUE;
        }
        return FALSE;
    }
    guint32 ulps = ABS (d_math_ulp_distancef(a, b));
    if (ulps <= max_ulps) {
        return TRUE;
    }
    return FALSE;
}

gboolean
d_math_equals_within_epsilon (gdouble   a,
                              gdouble   b,
                              gdouble   epsilon)
{
    gdouble diff = ABS(a-b);
    if (diff <= epsilon) {
        return TRUE;
    }
    return FALSE;
}

gboolean
d_math_equals_within_epsilonf (gfloat   a,
                               gfloat   b,
                               gfloat   epsilon)
{
    gfloat diff = ABS(a-b);
    if (diff <= epsilon) {
        return TRUE;
    }
    return FALSE;
}

gboolean
d_math_equals (gdouble a,
               gdouble b)
{
    if (d_math_equals_within_epsilon(a, b, FLT_EPSILON)) {
        return TRUE;
    }
    return d_math_equals_within_ulps(a, b, MAX_ULPS_DEFAULT);
}

gboolean
d_math_equalsf (gfloat  a,
                gfloat  b)
{
    if (d_math_equals_within_epsilonf(a, b, FLT_EPSILON)) {
        return TRUE;
    }
    return d_math_equals_within_ulpsf(a, b, MAX_ULPS_DEFAULT);
}
