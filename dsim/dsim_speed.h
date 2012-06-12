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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURSPEEDE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PROJECTNAME. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef  DSIM_SPEED_INC
#define  DSIM_SPEED_INC

#include <dsim/dsim_vector.h>

/*
 * Object representing the maximum speed allowed for each axis
 */

/* DSpeed type macros */
#define D_TYPE_SPEED             (d_speed_get_type ())
#define D_SPEED(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_SPEED, DSpeed))
#define D_IS_SPEED(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_SPEED))
#define D_SPEED_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_SPEED, DSpeedClass))
#define D_IS_SPEED_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_SPEED))
#define D_SPEED_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_SPEED, DSpeedClass))

/* Instance Structure of DSpeed */
typedef struct _DSpeed DSpeed;
struct _DSpeed {
    DVector         parent_instance;
};

/* Class Structure of DSpeed */
typedef struct _DSpeedClass DSpeedClass;
struct _DSpeedClass {
    DVectorClass    parent_class;
};

GType       d_speed_get_type    (void);

DVector*    d_speed_new         (void);

DVector*    d_speed_new_full    (gdouble    s1,
                                 gdouble    s2,
                                 gdouble    s3);

#endif   /* ----- #ifndef DSIM_SPEED_INC  ----- */

