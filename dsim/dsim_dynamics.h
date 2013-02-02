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
 * dsim_dynamics.h : Object with dynamic parameters of the Delta
 * manipulator.
 */


#ifndef  DSIM_DYNAMICS_INC
#define  DSIM_DYNAMICS_INC

#include <glib-object.h>
#include <math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_odeiv2.h>
#include <dsim/dsim_dynamic_spec.h>
#include <dsim/dsim_manipulator.h>
#include <dsim/dsim_solver.h>


/* DDynamicModel Type macros */
#define D_TYPE_DYNAMIC_MODEL             (d_dynamic_model_get_type ())
#define D_DYNAMIC_MODEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_DYNAMIC_MODEL, DDynamicModel))
#define D_IS_DYNAMIC_MODEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_DYNAMIC_MODEL))
#define D_DYNAMIC_MODEL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_DYNAMIC_MODEL, DDynamicModelClass))
#define D_IS_DYNAMIC_MODEL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_DYNAMIC_MODEL))
#define D_DYNAMIC_MODEL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_DYNAMIC_MODEL, DDynamicModelClass))

/* Instance Structure of DDynamicModel */
typedef struct _DDynamicModel DDynamicModel;
typedef struct _DDynamicModelPrivate DDynamicModelPrivate;
struct _DDynamicModel {
    GObject         parent_instance;

    /* Manipulator Asociated with this Dynamic Model */
    DManipulator    *manipulator;

    /* Force applied */
    gsl_vector      *force;

    /* Gravity acceleration */
    gsl_vector      *gravity;

    /* private */
    DDynamicModelPrivate *priv;
};

/* Class Structure of DDynamicModel */
typedef struct _DDynamicModelClass DDynamicModelClass;
struct _DDynamicModelClass {
    GObjectClass    parent_class;
};

/* Returns GType associated with this object type */
GType           d_dynamic_model_get_type    (void);

/* Methods */
DDynamicModel*  d_dynamic_model_new         (DManipulator   *manipulator);

void            d_dynamic_model_set_axes    (DDynamicModel  *self,
                                             gsl_vector     *axes);

gsl_vector*     d_dynamic_model_get_axes    (DDynamicModel  *self);

void            d_dynamic_model_set_speed   (DDynamicModel  *self,
                                             gsl_vector     *speed);

gsl_vector*     d_dynamic_model_get_speed   (DDynamicModel  *self);

void            d_dynamic_model_set_force   (DDynamicModel  *self,
                                             gsl_vector     *force);

gsl_vector*     d_dynamic_model_get_force   (DDynamicModel  *self);

void            d_dynamic_model_set_torque  (DDynamicModel  *self,
                                             gsl_vector     *torque);

gsl_vector*     d_dynamic_model_get_torque  (DDynamicModel  *self);

void            d_dynamic_model_set_gravity (DDynamicModel  *self,
                                             gsl_vector     *gravity);

gsl_vector*     d_dynamic_model_get_gravity (DDynamicModel  *self);

void            d_dynamic_model_solve_inverse
                                            (DDynamicModel  *self,
                                             gdouble        interval);

#endif   /* ----- #ifndef DSIM_DYNAMICS_INC  ----- */
