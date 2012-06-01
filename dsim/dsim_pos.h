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


#ifndef  DSIM_POS_INC
#define  DSIM_POS_INC

#include <dsim/dsim_vector.h>

/* Type macros */
#define D_TYPE_POS             (d_pos_get_type ())
#define D_POS(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_POS, DPos))
#define D_IS_POS(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_POS))
#define D_POS_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_POS, DPosClass))
#define D_IS_POS_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_POS))
#define D_POS_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_POS, DPosClass))

/* Instance Structure of DPos */
typedef struct _DPos DPos;
struct _DPos {
    DVector3        parent_instance;
};

/* Class Structure of DPos */
typedef struct _DPosClass DPosClass;
struct _DPosClass {
    DVector3Class   parent_class;
};

/* Returns GType associated with this object type */
GType       d_pos_get_type  (void);

/* Create new instance */
DVector3*   d_pos_new       (void);
DVector3*   d_pos_new_full  (gdouble x, gdouble y, gdouble z);

#endif   /* ----- #ifndef DSIM_POS_INC  ----- */

