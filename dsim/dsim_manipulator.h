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
 * dsim_manipulator.h : An object representing a delta robot manipulator
 * dynamics. Superclass for any real manipulator or dynamic simulator.
 */


#ifndef  DSIM_MANIPULATOR_INC
#define  DSIM_MANIPULATOR_INC

#include <glib-object.h>
#include <gsl/gsl_vector.h>
#include <dsim/dsim_geometry.h>
#include <dsim/dsim_dynamic_spec.h>

/* Type macros */
#define D_TYPE_MANIPULATOR             (d_manipulator_get_type ())
#define D_MANIPULATOR(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_MANIPULATOR, DManipulator))
#define D_IS_MANIPULATOR(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_MANIPULATOR))
#define D_MANIPULATOR_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_MANIPULATOR, DManipulatorClass))
#define D_IS_MANIPULATOR_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_MANIPULATOR))
#define D_MANIPULATOR_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_MANIPULATOR, DManipulatorClass))

/* Instance Structure of DManipulator */
typedef struct _DManipulator DManipulator;
struct _DManipulator {
    GObject         parent_instance;

    /* This manipulator physical parameters */
    DGeometry       *geometry;
    DDynamicSpec    *dynamic_params;

    /* Torques in the axes */
    gsl_vector      *torque;

    /* Axes for the manipulator's motors */
    gsl_vector      *axes;

    /* Speed for the manipulator's motors */
    gsl_vector      *speed;
};

/* Class Structure of DManipulator */
typedef struct _DManipulatorClass DManipulatorClass;
struct _DManipulatorClass {
    GObjectClass    parent_class;
};

/* Methods */
GType           d_manipulator_get_type      (void);

DManipulator*   d_manipulator_new           (DGeometry      *geometry,
                                             DDynamicSpec   *dynamic_params);

gdouble         d_manipulator_get_axis      (DManipulator   *self,
                                             gint           axis);

gsl_vector*     d_manipulator_get_axes      (DManipulator   *self);

void            d_manipulator_set_axis      (DManipulator   *self,
                                             gint           axis,
                                             gdouble        angle);

void            d_manipulator_set_axes      (DManipulator   *self,
                                             gsl_vector     *axes);

gdouble         d_manipulator_get_axis_speed(DManipulator   *self,
                                             gint           axis);

gsl_vector*     d_manipulator_get_speed     (DManipulator   *self);

void            d_manipulator_set_speed     (DManipulator   *self,
                                             gsl_vector     *speed);

gsl_vector*     d_manipulator_get_torque    (DManipulator   *self);

void            d_manipulator_set_torque    (DManipulator   *self,
                                             gsl_vector     *torque);

DDynamicSpec*   d_manipulator_get_dynamic_spec
                                            (DManipulator   *self);

DGeometry*      d_manipulator_get_geometry  (DManipulator   *self);

#endif   /* ----- #ifndef DSIM_MANIPULATOR_INC  ----- */
