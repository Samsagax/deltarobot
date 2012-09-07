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
#include <dsim/dsim_matrix.h>
#include <dsim/dsim_geometry.h>
#include <dsim/dsim_solver.h>

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

/* DDynamicModel Type macros */
#define D_TYPE_DYNAMIC_MODEL             (d_dynamic_model_get_type ())
#define D_DYNAMIC_MODEL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_DYNAMIC_MODEL, DDynamicModel))
#define D_IS_DYNAMIC_MODEL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_DYNAMIC_MODEL))
#define D_DYNAMIC_MODEL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_DYNAMIC_MODEL, DDynamicModelClass))
#define D_IS_DYNAMIC_MODEL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_DYNAMIC_MODEL))
#define D_DYNAMIC_MODEL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_DYNAMIC_MODEL, DDynamicModelClass))

/* Instance Structure of DDynamicModel */
typedef struct _DDynamicModel DDynamicModel;
struct _DDynamicModel {
    GObject         parent_instance;

    /* Geometric and dynamic parameters */
    DDynamicSpec    *dynamic_spec;
    DGeometry       *geometry;

    /* Position and Speed in axes space */
    DVector         *axes;
    DVector         *speed;

    /* Force and Torque applied */
    DVector         *force;
    DVector         *torque;

    /* Gravity acceleration */
    DVector         *gravity;

    /* Some useful singleton matrices and flags to update them */
    DMatrix         *jacobian_p_inv;    /* Direct Jacobian Matrix */
    DMatrix         *jacobian_p_dot;    /* Direct Jacobian Derivative */
    DMatrix         *jacobian_q;        /* Inverse Jacobian Matrix */
    DMatrix         *jacobian_q_dot;    /* Inverse Jacobian Derivative */

    gboolean        jpi_update;
    gboolean        jpd_update;
    gboolean        jq_update;
    gboolean        jqd_update;

    DMatrix         *mass_axes;
    DMatrix         *mass_pos;
    DMatrix         *inertia_axes;

    gboolean        ma_update;
    gboolean        mp_update;
    gboolean        ia_update;

    /* Model singleton matrices and update flags */
    DMatrix         *model_mass;
    DMatrix         *model_coriolis;
    DMatrix         *model_inertia_inv;
    DVector         *model_torque;

    gboolean        mm_update;
    gboolean        mc_update;
    gboolean        mi_update;
    gboolean        mt_update;
};

/* Class Structure of DDynamicModel */
typedef struct _DDynamicModelClass DDynamicModelClass;
struct _DDynamicModelClass {
    GObjectClass    parent_class;
};

/* Returns GType associated with this object type */
GType           d_dynamic_model_get_type    (void);

/* Methods */
DDynamicModel*  d_dynamic_model_new         (DGeometry      *geometry,
                                             DDynamicSpec   *dynamic_spec);

void            d_dynamic_model_set_axes    (DDynamicModel  *self,
                                             DVector        *axes);

DVector*        d_dynamic_model_get_axes    (DDynamicModel  *self);

void            d_dynamic_model_set_speed   (DDynamicModel  *self,
                                             DVector        *speed);

DVector*        d_dynamic_model_get_speed   (DDynamicModel  *self);

void            d_dynamic_model_set_force   (DDynamicModel  *self,
                                             DVector        *force);

DVector*        d_dynamic_model_get_force   (DDynamicModel  *self);

void            d_dynamic_model_set_torque  (DDynamicModel  *self,
                                             DVector        *torque);

DVector*        d_dynamic_model_get_torque  (DDynamicModel  *self);

void            d_dynamic_model_solve_inverse_axes
                                            (DDynamicModel  *self);

#endif   /* ----- #ifndef DSIM_DYNAMICS_INC  ----- */
