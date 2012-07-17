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
 * dsim_jacobian.h: Objects representing direct/inverse DirectJacobian of the manipulator
 */

#ifndef  DSIM_DIRECT_JACOBIAN_INC
#define  DSIM_DIRECT_JACOBIAN_INC

#include <dsim/dsim_matrix.h>
#include <dsim/dsim_axes.h>

/* Type macros */
#define D_TYPE_JACOBIAN             (d_jacobian_get_type ())
#define D_JACOBIAN(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_JACOBIAN, DJacobian))
#define D_IS_JACOBIAN(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_JACOBIAN))
#define D_JACOBIAN_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_JACOBIAN, DJacobianClass))
#define D_IS_JACOBIAN_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_JACOBIAN))
#define D_JACOBIAN_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_JACOBIAN, DJacobianClass))

/* Instance Structure of DJacobian */
typedef struct _DJacobian DJacobian;
struct _DJacobian {
    DMatrix         parent_instance;
};

/* Class Structure of DJacobian */
typedef struct _DJacobianClass DJacobianClass;
struct _DJacobianClass {
    DMatrixClass    parent_class;

    /* Virtual methods */
    gboolean    (*is_singular)      (DJacobian  *self);
};

GType       d_jacobian_get_type     (void);

gboolean    d_jacobian_is_singular  (DJacobian  *self);

/* Type macros */
#define D_TYPE_DIRECT_JACOBIAN             (d_direct_jacobian_get_type ())
#define D_DIRECT_JACOBIAN(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_DIRECT_JACOBIAN, DDirectJacobian))
#define D_IS_DIRECT_JACOBIAN(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_DIRECT_JACOBIAN))
#define D_DIRECT_JACOBIAN_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_DIRECT_JACOBIAN, DDirectJacobianClass))
#define D_IS_DIRECT_JACOBIAN_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_DIRECT_JACOBIAN))
#define D_DIRECT_JACOBIAN_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_DIRECT_JACOBIAN, DDirectJacobianClass))

/* Instance Structure of DDirectJacobian */
typedef struct _DDirectJacobian DDirectJacobian;
struct _DDirectJacobian {
    DJacobian           parent_instance;
};

/* Class Structure of DDirectJacobian */
typedef struct _DDirectJacobianClass DDirectJacobianClass;
struct _DDirectJacobianClass {
    DJacobianClass      parent_class;
};

GType       d_direct_jacobian_get_type      (void);

DMatrix*    d_direct_jacobian_new           (DExtAxes    *axes);

/* Type macros */
#define D_TYPE_INVERSE_JACOBIAN             (d_inverse_jacobian_get_type ())
#define D_INVERSE_JACOBIAN(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_INVERSE_JACOBIAN, DInverseJacobian))
#define D_IS_INVERSE_JACOBIAN(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_INVERSE_JACOBIAN))
#define D_INVERSE_JACOBIAN_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_INVERSE_JACOBIAN, DInverseJacobianClass))
#define D_IS_INVERSE_JACOBIAN_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_INVERSE_JACOBIAN))
#define D_INVERSE_JACOBIAN_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_INVERSE_JACOBIAN, DInverseJacobianClass))

/* Instance Structure of DInverseJacobian */
typedef struct _DInverseJacobian DInverseJacobian;
struct _DInverseJacobian {
    DJacobian       parent_instance;
};

/* Class Structure of DInverseJacobian */
typedef struct _DInverseJacobianClass DInverseJacobianClass;
struct _DInverseJacobianClass {
    DJacobianClass  parent_class;
};

GType       d_inverse_jacobian_get_type     (void);

DMatrix*    d_inverse_jacobian_new          (DExtAxes    *axes);

#endif   /* ----- #ifndef DSIM_DIRECT_JACOBIAN_INC  ----- */

