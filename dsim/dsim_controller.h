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
 * dsim_controller.h: Object representing a cntroller for robot axes working
 *      in an open loop.
 *      A function is provided to close the loop in implementations
 */

#ifndef  DSIM_CONTROLLER_INC
#define  DSIM_CONTROLLER_INC

#include <glib-object.h>
#include <dsim/dsim_axes.h>
#include <dsim/dsim_speed.h>

/* Type macros */
#define D_TYPE_CONTROLLER             (d_controller_get_type ())
#define D_CONTROLLER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_CONTROLLER, DController))
#define D_IS_CONTROLLER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_CONTROLLER))
#define D_CONTROLLER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_CONTROLLER, DControllerClass))
#define D_IS_CONTROLLER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_CONTROLLER))
#define D_CONTROLLER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_CONTROLLER, DControllerClass))

/**
 * Control Transfer function
 * Should convert Position and speed into torque in an open-loop
 */
typedef DVector* (*DControlTransFunc) (DVector*, DVector*);

/* Instance Structure of DController */
typedef struct _DController DController;
struct _DController {
    GObject         parent_instance;

    /* Current set point signal */
    DVector         *set_pos;
    DVector         *set_speed;

    /* Current inputs signal */
    DVector         *axes_pos;
    DVector         *axes_speed;

    /* Current torque signal */
    DVector         *torque;

    /* Controller transfer signal */
    DControlTransFunc   trans_func;
};

/* Class Structure of DController */
typedef struct _DControllerClass DControllerClass;
struct _DControllerClass {
    GObjectClass    parent_class;
};

/* Methods */
GType           d_controller_get_type       (void);

DController*    d_controller_new            (void);

DVector*        d_controller_get_torque     (DController        *self);

void            d_controller_set_set_point  (DController        *self,
                                             DVector            *set_pos,
                                             DVector            *set_speed);

void            d_controller_set_input      (DController        *self,
                                             DVector            *axes_pos,
                                             DVector            *axes_speed);

void            d_controller_set_trans_func (DController        *self,
                                             DControlTransFunc  trans_func);

#endif   /* ----- #ifndef DSIM_CONTROLLER_INC  ----- */
