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
 * dsim_dynamic_spec.h : Object with dynamic parameters of the Delta
 * manipulator.
 */

#ifndef  DSIM_DYNAMIC_SPEC_INC
#define  DSIM_DYNAMIC_SPEC_INC

#include <glib-object.h>

/* DDynamicSpec Type macros */
#define D_TYPE_DYNAMIC_SPEC             (d_dynamic_spec_get_type ())
#define D_DYNAMIC_SPEC(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_DYNAMIC_SPEC, DDynamicSpec))
#define D_IS_DYNAMIC_SPEC(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_DYNAMIC_SPEC))
#define D_DYNAMIC_SPEC_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_DYNAMIC_SPEC, DDynamicSpecClass))
#define D_IS_DYNAMIC_SPEC_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_DYNAMIC_SPEC))
#define D_DYNAMIC_SPEC_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_DYNAMIC_SPEC, DDynamicSpecClass))

/* Instance Structure of DDynamicSpec */
typedef struct _DDynamicSpec DDynamicSpec;
struct _DDynamicSpec {
    GObject         parent_instance;

    gdouble         low_arm_mass;
    gdouble         low_arm_moi;
    gdouble         upper_arm_mass;
    gdouble         upper_arm_moi;  /* not used */
    gdouble         platform_mass;
};

/* Class Structure of DDynamicSpec */
typedef struct _DDynamicSpecClass DDynamicSpecClass;
struct _DDynamicSpecClass {
    GObjectClass    parent_class;
};

/* Returns GType associated with this object type */
GType       d_dynamic_spec_get_type     (void);

/* Methods */
DDynamicSpec*  d_dynamic_spec_new       (void);

#endif   /* ----- #ifndef dsim_dynamic_spec_INC  ----- */
