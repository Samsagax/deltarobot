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
 * dsim_dynamic_model.c :
 */

#include <gsl/gsl_odeiv2.h>
#include "dsim_dynamics.h"

/* Forward declarations */
static void         d_dynamic_model_class_init      (DDynamicModelClass  *klass);

static void         d_dynamic_model_init            (DDynamicModel       *self);

static void         d_dynamic_model_dispose         (GObject            *obj);

static void         d_dynamic_model_finalize        (GObject            *obj);

static void         d_dynamic_model_set_geometry    (DDynamicModel  *self,
                                                     DGeometry      *geometry);

static void         d_dynamic_model_set_dynamic_spec(DDynamicModel  *self,
                                                     DDynamicSpec   *dynamic_spec);

static DMatrix*     d_dynamic_model_get_direct_jacobian_dt
                                                    (DDynamicModel  *self);

static DMatrix*     d_dynamic_model_get_direct_jacobian_inv
                                                    (DDynamicModel  *self);

static DMatrix*     d_dynamic_model_get_inverse_jacobian
                                                    (DDynamicModel  *self);

static DMatrix*     d_dynamic_model_get_inverse_jacobian_dt
                                                    (DDynamicModel  *self);

static DMatrix*     d_dynamic_model_get_inertia_axes(DDynamicModel  *self);

static DMatrix*     d_dynamic_model_get_mass_pos    (DDynamicModel  *self);

static DMatrix*     d_dynamic_model_get_mass_axes   (DDynamicModel  *self);

static DMatrix*     d_dynamic_model_get_model_inertia_inv
                                                    (DDynamicModel  *self);

static DMatrix*     d_dynamic_model_get_model_mass  (DDynamicModel  *self);

static DMatrix*     d_dynamic_model_get_model_coriolis
                                                    (DDynamicModel  *self);

static DVector*     d_dynamic_model_get_model_torque(DDynamicModel  *self);

/* GType Register */
G_DEFINE_TYPE(DDynamicModel, d_dynamic_model, G_TYPE_OBJECT);

/* DDynamicModel implementaion */
static void
d_dynamic_model_init (DDynamicModel   *self)
{
    self->manipulator = NULL;

    self->force = d_vector_new(3);
    self->gravity = d_vector_new(3);

    self->jacobian_p_inv = NULL;
    self->jacobian_p_dot = NULL;
    self->jacobian_q = NULL;
    self->jacobian_q_dot = NULL;

    self->jpd_update = TRUE;
    self->jq_update = TRUE;
    self->jpi_update = TRUE;
    self->jqd_update = TRUE;

    self->mass_axes = NULL;
    self->mass_pos = NULL;
    self->inertia_axes = NULL;

    self->ma_update = TRUE;
    self->mp_update = TRUE;
    self->ia_update = TRUE;

    self->model_mass = NULL;
    self->model_coriolis = NULL;
    self->model_inertia_inv = NULL;
    self->model_torque = NULL;

    self->mm_update = TRUE;
    self->mc_update = TRUE;
    self->mi_update = TRUE;
    self->mt_update = TRUE;
}

static void
d_dynamic_model_class_init (DDynamicModelClass    *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_dynamic_model_dispose;
    gobject_class->finalize = d_dynamic_model_finalize;
}

static void
d_dynamic_model_dispose (GObject *gobject)
{
    DDynamicModel *self = D_DYNAMIC_MODEL(gobject);

    if (self->manipulator) {
        g_object_unref(self->manipulator);
        self->manipulator = NULL;
    }
    if (self->force) {
        g_object_unref(self->force);
        self->force = NULL;
    }
    if (self->gravity) {
        g_object_unref(self->gravity);
        self->gravity = NULL;
    }
    if (self->jacobian_p_inv) {
        g_object_unref(self->jacobian_p_inv);
        self->jacobian_p_inv = NULL;
    }
    if (self->jacobian_p_dot) {
        g_object_unref(self->jacobian_p_dot);
        self->jacobian_p_dot = NULL;
    }
    if (self->jacobian_q) {
        g_object_unref(self->jacobian_q);
        self->jacobian_q = NULL;
    }
    if (self->jacobian_q_dot) {
        g_object_unref(self->jacobian_q_dot);
        self->jacobian_q_dot = NULL;
    }
    if (self->mass_axes) {
        g_object_unref(self->mass_axes);
        self->mass_axes = NULL;
    }
    if (self->mass_pos) {
        g_object_unref(self->mass_pos);
        self->mass_pos = NULL;
    }
    if (self->inertia_axes) {
        g_object_unref(self->inertia_axes);
        self->inertia_axes = NULL;
    }
    if (self->model_mass) {
        g_object_unref(self->model_mass);
        self->model_mass = NULL;
    }
    if (self->model_coriolis) {
        g_object_unref(self->model_coriolis);
        self->model_coriolis = NULL;
    }
    if (self->model_inertia_inv) {
        g_object_unref(self->model_inertia_inv);
        self->model_inertia_inv = NULL;
    }
    if (self->model_torque) {
        g_object_unref(self->model_torque);
        self->model_torque = NULL;
    }

    /*  Chain Up */
    G_OBJECT_CLASS(d_dynamic_model_parent_class)->dispose(gobject);
}

static void
d_dynamic_model_finalize (GObject *gobject)
{
    G_OBJECT_CLASS (d_dynamic_model_parent_class)->finalize(gobject);
}

static void
d_dynamic_model_set_manipulator (DDynamicModel  *self,
                                 DManipulator   *manipulator)
{
    if (self->manipulator) {
        g_object_unref(self->manipulator);
    }
    self->manipulator = g_object_ref(manipulator);
}

/*
 * Update direct jacobian inverse matrix with current parameters
 */
static void
d_dynamic_model_update_jpi (DDynamicModel   *self)
{
    if (self->jacobian_p_inv == NULL) {
        self->jacobian_p_inv = d_matrix_new(3, 3);
    }

    DVector *axes = d_manipulator_get_axes(self->manipulator);
    DVector *pos = d_vector_new(3);
    gsl_matrix *jpi = self->jacobian_p_inv->matrix;
    DGeometry *geometry = d_manipulator_get_geometry(self->manipulator);
    gdouble a = geometry->a;
    gdouble h = geometry->h;
    gdouble r = geometry->r;

    gsl_matrix_set_all(jpi, 0);
    d_solver_solve_direct(geometry, axes, pos);
    for (int i = 0; i < jpi->size1; i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (gdouble)i;
        gdouble t = d_vector_get(axes, i);
        gdouble gammax = 2.0 * (d_vector_get(pos, 0) + (h-r) * cos(phi)
                                - a * cos(phi) * cos(t));
        gdouble gammay = 2.0 * (d_vector_get(pos, 1) + (h-r) * sin(phi)
                                - a * sin(phi) * cos(t));
        gdouble gammaz = 2.0 * (d_vector_get(pos, 2) - a * sin(t));
        gsl_matrix_set(jpi, i, 0, - gammax);
        gsl_matrix_set(jpi, i, 1, - gammay);
        gsl_matrix_set(jpi, i, 2, - gammaz);
    }
    d_matrix_inverse(self->jacobian_p_inv);

    self->jpi_update = FALSE;

    g_object_unref(pos);
}

/*
 * Update direct jacobian derivative matrix with current parameters
 */
static void
d_dynamic_model_update_jpd (DDynamicModel   *self)
{
    if (self->jacobian_p_dot == NULL) {
        self->jacobian_p_dot = d_matrix_new(3, 3);
    }

    DVector *axes = d_manipulator_get_axes(self->manipulator);
    DVector *pos = d_vector_new(3);
    DVector *speed_axes = d_manipulator_get_speed(self->manipulator);
    DVector *speed_pos = d_vector_new(3);
    DGeometry *geometry = d_manipulator_get_geometry(self->manipulator);
    DMatrix *jp_inv = d_dynamic_model_get_direct_jacobian_inv(self);
    DMatrix *jq = d_dynamic_model_get_inverse_jacobian(self);
    gsl_matrix *jpd = self->jacobian_p_dot->matrix;
    gdouble a = geometry->a;
    gsl_vector *temp = gsl_vector_calloc(3);

    gsl_matrix_set_all(jpd, 0);
    d_solver_solve_direct(geometry, axes, pos);
    gsl_blas_dgemv(CblasNoTrans, 1.0, jq->matrix,
                    speed_axes->vector, 0.0, temp);
    gsl_blas_dgemv(CblasNoTrans, 1.0, jp_inv->matrix,
                    temp, 0.0, speed_pos->vector);
    for (int i = 0; i < jpd->size1; i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (double)i;
        gdouble t = d_vector_get(axes, i);
        gdouble t_dot = d_vector_get(speed_axes, i);

        gdouble gammax_dot = 2.0 * (d_vector_get(speed_pos, 0)
                                + a * cos(phi) * sin(t) * t_dot);
        gdouble gammay_dot = 2.0 * (d_vector_get(speed_pos, 1)
                                + a * sin(phi) * sin(t) * t_dot);
        gdouble gammaz_dot = 2.0 * (d_vector_get(speed_pos, 2)
                                - a * cos(t) * t_dot);

        gsl_matrix_set(jpd, i, 0, - gammax_dot);
        gsl_matrix_set(jpd, i, 1, - gammay_dot);
        gsl_matrix_set(jpd, i, 2, - gammaz_dot);
    }

    self->jpd_update = FALSE;

    g_object_unref(speed_pos);
    g_object_unref(pos);
    gsl_vector_free(temp);
}

/*
 * Update inverse jacobian matrix with current parameters
 */
static void
d_dynamic_model_update_jq (DDynamicModel   *self)
{
    if (self->jacobian_q == NULL) {
        self->jacobian_q = d_matrix_new(3, 3);
    }

    DVector *axes = d_manipulator_get_axes(self->manipulator);
    DVector *pos = d_vector_new(3);
    gsl_matrix *jq = self->jacobian_q->matrix;
    DGeometry* geometry = d_manipulator_get_geometry(self->manipulator);
    gdouble a = geometry->a;
    gdouble h = geometry->h;
    gdouble r = geometry->r;

    gsl_matrix_set_all(jq, 0);
    d_solver_solve_direct(geometry, axes, pos);
    for (int i = 0; i < jq->size1; i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (double)i;
        gdouble t = d_vector_get(axes, i);
        gsl_matrix_set(jq, i, i,
                        2.0 * a * ((d_vector_get(pos, 0) * cos(phi)
                                + d_vector_get(pos, 1) * sin(phi)
                                + h - r)
                                * sin(t)
                                - d_vector_get(pos, 2) * cos(t)));
    }

    self->jq_update = FALSE;

    g_object_unref(pos);
}

/*
 * Update inverse jacobian derivative matrix with current parameters
 */
static void
d_dynamic_model_update_jqd (DDynamicModel   *self)
{
    if (self->jacobian_q_dot == NULL) {
        self->jacobian_q_dot = d_matrix_new(3, 3);
    }

    DVector *axes = d_manipulator_get_axes(self->manipulator);
    DVector *pos = d_vector_new(3);
    DVector *speed_axes = d_manipulator_get_speed(self->manipulator);
    DVector *speed_pos = d_vector_new(3);
    DGeometry *geometry = d_manipulator_get_geometry(self->manipulator);
    DMatrix *jp_inv = d_dynamic_model_get_direct_jacobian_inv(self);
    DMatrix *jq = d_dynamic_model_get_inverse_jacobian(self);

    gsl_matrix *jqd = self->jacobian_q_dot->matrix;
    gdouble a = geometry->a;
    gdouble h = geometry->h;
    gdouble r = geometry->r;
    gsl_vector *temp = gsl_vector_calloc(3);

    gsl_matrix_set_all(jqd, 0);
    d_solver_solve_direct(geometry, axes, pos);
    gsl_blas_dgemv(CblasNoTrans, 1.0, jq->matrix,
                    speed_axes->vector, 0.0, temp);
    gsl_blas_dgemv(CblasNoTrans, 1.0, jp_inv->matrix,
                    temp, 0.0, speed_pos->vector);
    for (int i = 0; i < jqd->size1 ; i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (gdouble)i;
        gdouble t = d_vector_get(axes, i);
        gdouble t_dot = d_vector_get(speed_axes, i);
        gsl_matrix_set(jqd, i, i,
                        2.0 * a * (
                            (d_vector_get(speed_pos, 0) * cos(phi)
                                + d_vector_get(speed_pos, 1) * sin(phi)
                                + d_vector_get(pos,2) * t_dot) * sin(t)
                            + ((d_vector_get(pos, 0) * cos(phi)
                                + d_vector_get(pos, 1) * sin(phi)
                                + h - r) * t_dot
                                - d_vector_get(speed_pos, 2)) * cos(t)));
    }

    self->jqd_update = FALSE;

    g_object_unref(speed_pos);
    g_object_unref(pos);
    gsl_vector_free(temp);
}

/*
 * Update axes mass matrix with current parameters
 */
static void
d_dynamic_model_update_mass_axes (DDynamicModel *self)
{
    if (self->mass_axes == NULL) {
        self->mass_axes = d_matrix_new(3, 3);
    }
    gsl_matrix *ma = self->mass_axes->matrix;
    DVector *axes = d_manipulator_get_axes(self->manipulator);

    gsl_matrix_set_all(ma, 0);
    for (int i = 0; i < ma->size1; i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (double)i;
        gdouble t = d_vector_get(axes, i);
        gsl_matrix_set(ma, i, 0, - cos(phi) * sin(t));
        gsl_matrix_set(ma, i, 1, - sin(phi) * sin(t));
        gsl_matrix_set(ma, i, 2, cos(t));
    }
    gdouble mass = (self->manipulator->dynamic_params->low_arm_mass / 2.0
                    + self->manipulator->dynamic_params->upper_arm_mass)
                    * self->manipulator->geometry->a;
    gsl_matrix_scale(ma, mass);

    self->ma_update = FALSE;
}

/*
 * Update axes mass matrix with current parameters
 */
static void
d_dynamic_model_update_mass_pos (DDynamicModel  *self)
{
    if (self->mass_pos == NULL) {
        self->mass_pos = d_matrix_new(3, 3);
    }
    gsl_matrix *mp = self->mass_pos->matrix;

    gsl_matrix_set_all(mp, 0);
    for (int i = 0; i < mp->size1; i++) {
        gdouble mass = self->manipulator->dynamic_params->platform_mass
                        + 3.0 * self->manipulator->dynamic_params->upper_arm_mass;
        gsl_matrix_set(mp, i, i, mass);
    }

    self->mp_update = FALSE;
}

/*
 * Update axes inertia matrix with current parameters
 */
static void
d_dynamic_model_update_inertia_axes (DDynamicModel  *self)
{
    if (self->inertia_axes == NULL) {
        self->inertia_axes = d_matrix_new(3, 3);
    }
    gsl_matrix *ia = self->inertia_axes->matrix;

    gsl_matrix_set_all(ia, 0);
    for (int i = 0; i < ia->size1; i++) {
        gdouble mass = self->manipulator->dynamic_params->upper_arm_mass
                        * pow(self->manipulator->geometry->a, 2.0)
                        + self->manipulator->dynamic_params->low_arm_moi;
        gsl_matrix_set(ia, i, i, mass);
    }

    self->ia_update = FALSE;
}

/*
 * Update model mass matrix with current parameters
 */
static void
d_dynamic_model_update_model_mass (DDynamicModel    *self)
{
    if (self->model_mass == NULL) {
        self->model_mass = d_matrix_new(3, 3);
    }
    gsl_matrix *mass = self->model_mass->matrix;
    DMatrix *jp = d_dynamic_model_get_direct_jacobian_inv(self);
    DMatrix *jq = d_dynamic_model_get_inverse_jacobian(self);
    DMatrix *mp = d_dynamic_model_get_mass_pos(self);
    DMatrix *mq = d_dynamic_model_get_mass_axes(self);
    gsl_matrix *temp = gsl_matrix_calloc(3, 3);

    gsl_matrix_set_all(mass, 0);

    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0,
                    jq->matrix, jp->matrix, 0.0, temp);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    temp, mp->matrix, 0.0, mass);
    gsl_matrix_add(mass, mq->matrix);

    self->mm_update = FALSE;

    gsl_matrix_free(temp);
}

/*
 * Update model inertia matrix with current parameters
 */
static void
d_dynamic_model_update_model_inertia_inv (DDynamicModel *self)
{
    if (self->model_inertia_inv == NULL) {
        self->model_inertia_inv = d_matrix_new(3, 3);
    }
    gsl_matrix *inertia = self->model_inertia_inv->matrix;
    DMatrix *jp = d_dynamic_model_get_direct_jacobian_inv(self);
    DMatrix *jq = d_dynamic_model_get_inverse_jacobian(self);
    DMatrix *mp = d_dynamic_model_get_mass_pos(self);
    DMatrix *iq = d_dynamic_model_get_inertia_axes(self);
    gsl_matrix *temp = gsl_matrix_calloc(3, 3);

    gsl_matrix_set_all(inertia, 0.0);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0,
                    jq->matrix, jp->matrix,
                    0.0, temp);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    temp, mp->matrix,
                    0.0, inertia);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    inertia, jp->matrix,
                    0.0, temp);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    temp, jq->matrix,
                    0.0, inertia);
    gsl_matrix_add(inertia, iq->matrix);
    d_matrix_inverse(self->model_inertia_inv);

    self->mi_update = FALSE;

    gsl_matrix_free(temp);
}

/*
 * Update model coriolis matrix with current parameters
 */
static void
d_dynamic_model_update_model_coriolis (DDynamicModel   *self)
{
    if (self->model_coriolis == NULL) {
        self->model_coriolis = d_matrix_new(3, 3);
    }
    gsl_matrix *coriolis = self->model_coriolis->matrix;
    DMatrix *jp = d_dynamic_model_get_direct_jacobian_inv(self);
    DMatrix *jq = d_dynamic_model_get_inverse_jacobian(self);
    DMatrix *jpd = d_dynamic_model_get_direct_jacobian_dt(self);
    DMatrix *jqd = d_dynamic_model_get_inverse_jacobian_dt(self);
    DMatrix *mp = d_dynamic_model_get_mass_pos(self);
    DMatrix *term1 = d_matrix_new(3, 3);
    DMatrix *term2 = d_matrix_new(3, 3);
    gsl_matrix *temp = gsl_matrix_calloc(3, 3);

    gsl_matrix_set_all(coriolis, 0.0);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    jp->matrix, jqd->matrix,
                    0.0, term1->matrix);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    jp->matrix, jpd->matrix,
                    0.0, term2->matrix);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    term2->matrix, jp->matrix,
                    0.0, temp);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    temp, jq->matrix,
                    0.0, term2->matrix);
    gsl_matrix_add(term1->matrix, term2->matrix);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0,
                    jq->matrix, jp->matrix,
                    0.0, coriolis);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    coriolis, mp->matrix,
                    0.0, temp);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    temp, term1->matrix,
                    0.0, coriolis);

    g_object_unref(term1);
    g_object_unref(term2);
    gsl_matrix_free(temp);

    self->mc_update = FALSE;
}

/*
 * Update model torque vector with current parameters
 */
static void
d_dynamic_model_update_model_torque (DDynamicModel  *self)
{
    if (self->model_torque == NULL) {
        self->model_torque = d_vector_new(3);
    }

    gsl_vector *torque = self->model_torque->vector;
    DVector *ff = self->force;
    DVector *tt = d_manipulator_get_torque(self->manipulator);
    DMatrix *jp = d_dynamic_model_get_direct_jacobian_inv(self);
    DMatrix *jq = d_dynamic_model_get_inverse_jacobian(self);
    gsl_vector *temp = gsl_vector_calloc(3);

    gsl_vector_set_all(torque, 0.0);
    gsl_blas_dgemv(CblasTrans, 1.0,
                    jp->matrix, ff->vector, 0.0, temp);
    gsl_blas_dgemv(CblasNoTrans, 1.0,
                    jq->matrix, temp, 0.0, torque);
    gsl_vector_add(torque, tt->vector);

    self->mt_update = FALSE;

    gsl_vector_free(temp);
}

static DMatrix*
d_dynamic_model_get_inverse_jacobian_dt (DDynamicModel  *self)
{
    if (self->jqd_update) {
        d_dynamic_model_update_jqd(self);
    }
    return self->jacobian_q_dot;
}

static DMatrix*
d_dynamic_model_get_inverse_jacobian (DDynamicModel *self)
{
    if (self->jq_update) {
        d_dynamic_model_update_jq(self);
    }
    return self->jacobian_q;
}

static DMatrix*
d_dynamic_model_get_direct_jacobian_dt (DDynamicModel   *self)
{
    if (self->jpd_update) {
        d_dynamic_model_update_jpd(self);
    }
    return self->jacobian_p_dot;
}

static DMatrix*
d_dynamic_model_get_direct_jacobian_inv (DDynamicModel  *self)
{
    if (self->jpi_update) {
        d_dynamic_model_update_jpi(self);
    }
    return self->jacobian_p_inv;
}

static DMatrix*
d_dynamic_model_get_inertia_axes (DDynamicModel *self)
{
    if (self->ia_update) {
        d_dynamic_model_update_inertia_axes(self);
    }

    return self->inertia_axes;
}

static DMatrix*
d_dynamic_model_get_mass_pos (DDynamicModel     *self)
{
    if (self->mp_update) {
        d_dynamic_model_update_mass_pos(self);
    }

    return self->mass_pos;
}

static DMatrix*
d_dynamic_model_get_mass_axes (DDynamicModel   *self)
{
    if (self->ma_update) {
        d_dynamic_model_update_mass_axes(self);
    }

    return self->mass_axes;
}

static DMatrix*
d_dynamic_model_get_model_inertia_inv (DDynamicModel    *self)
{
    if (self->mi_update) {
        d_dynamic_model_update_model_inertia_inv(self);
    }

    return self->model_inertia_inv;
}

static DMatrix*
d_dynamic_model_get_model_mass (DDynamicModel   *self)
{
    if (self->mm_update) {
        d_dynamic_model_update_model_mass(self);
    }

    return self->model_mass;
}

static DMatrix*
d_dynamic_model_get_model_coriolis (DDynamicModel   *self)
{
    if (self->mc_update) {
        d_dynamic_model_update_model_coriolis(self);
    }

    return self->model_coriolis;
}

static DVector*
d_dynamic_model_get_model_torque (DDynamicModel *self)
{
    if (self->mt_update) {
        d_dynamic_model_update_model_torque(self);
    }

    return self->model_torque;
}

/**
 * Function defining the ODE system. The first three elements of array y[]
 * are the positions in axes-space. The remaining three elements are the
 * speeds in the axes-space.
 *
 * Parameters passed to the function in the param argument must be a valid
 * DDynamicModel object.
 */
static int
d_dynamic_model_equation (double        t,
                          const double  y[],
                          double        dydt[],
                          void          *params)
{
    /* Check input data for correct type */
    g_return_val_if_fail(D_IS_DYNAMIC_MODEL(params), GSL_EBADFUNC);

    DDynamicModel *model = D_DYNAMIC_MODEL(params);

    /* Matrices holding the coefficients on the model differential equation */
    DMatrix *mi, *mh, *mm;
    DVector *mt;

    /* Speed and positions in axes space */
    DVector *q, *q_dot, *q_dot_dot, *q_dot_dot_temp;

    /* Fill the axes position and speed to request an update in the model */
    q = d_vector_new(3);
    q_dot = d_vector_new(3);
    for (int i = 0; i < 3; i++) {
        d_vector_set(q, i, y[i]);
        d_vector_set(q_dot, i, y[i+3]);
    }
    d_dynamic_model_set_axes(model, q);
    d_dynamic_model_set_speed(model, q_dot);

    /* Fill model matrices */
    mt = d_dynamic_model_get_model_torque(model);
    mi = d_dynamic_model_get_model_inertia_inv(model);
    mh = d_dynamic_model_get_model_coriolis(model);
    mm = d_dynamic_model_get_model_mass(model);

    /* Calculate acceleration */
    /* d²q/dt² = I⁻¹ ( T - H * dq/dt + M * g ) */
    q_dot_dot = d_vector_new(3);
    q_dot_dot_temp = d_vector_new(3);
    gsl_vector *v_q_dot_dot = q_dot_dot->vector;
    gsl_vector *v_q_dot_dot_temp = q_dot_dot_temp->vector;
    gsl_vector *v_q_dot = q_dot->vector;
    gsl_vector *v_g = model->gravity->vector;
    gsl_vector *v_mt = mt->vector;
    gsl_matrix *m_mi = mi->matrix;
    gsl_matrix *m_mh = mh->matrix;
    gsl_matrix *m_mm = mm->matrix;

    gsl_blas_dgemv(CblasNoTrans, 1.0, m_mm, v_g, 0.0, v_q_dot_dot_temp);
    gsl_blas_dgemv(CblasNoTrans, -1.0, m_mh, v_q_dot, 1.0, v_q_dot_dot_temp);
    gsl_vector_add(v_q_dot_dot_temp, v_mt);
    gsl_blas_dgemv(CblasNoTrans, 1.0, m_mi, v_q_dot_dot_temp, 0.0, v_q_dot_dot);

    /* Set the dydt array */
    dydt[0] = d_vector_get(q_dot, 0);
    dydt[1] = d_vector_get(q_dot, 1);
    dydt[2] = d_vector_get(q_dot, 2);
    dydt[3] = d_vector_get(q_dot_dot, 0);
    dydt[4] = d_vector_get(q_dot_dot, 1);
    dydt[5] = d_vector_get(q_dot_dot, 2);

    /* Free resources allocated during this step */
    g_object_unref(q);
    g_object_unref(q_dot);
    g_object_unref(q_dot_dot);

    return GSL_SUCCESS;
}

/* Public API */
DDynamicModel*
d_dynamic_model_new (DManipulator   *manipulator)
{
    DDynamicModel *dm = g_object_new(D_TYPE_DYNAMIC_MODEL, NULL);

    d_dynamic_model_set_manipulator(dm, manipulator);

    return dm;
}

DVector*
d_dynamic_model_get_axes (DDynamicModel *self)
{
    return d_manipulator_get_axes(self->manipulator);
}

void
d_dynamic_model_set_axes (DDynamicModel *self,
                          DVector       *axes)
{
    d_manipulator_set_axes(self->manipulator, axes);
    self->jpi_update = TRUE;
    self->jpd_update = TRUE;
    self->jq_update = TRUE;
    self->jqd_update = TRUE;
    self->ma_update = TRUE;
    self->mp_update = TRUE;
    self->ia_update = TRUE;
    self->mm_update = TRUE;
    self->mc_update = TRUE;
    self->mi_update = TRUE;
    self->mt_update = TRUE;
}

DVector*
d_dynamic_model_get_speed (DDynamicModel    *self)
{
    return d_manipulator_get_speed(self->manipulator);
}

void
d_dynamic_model_set_speed (DDynamicModel    *self,
                           DVector          *speed)
{
    d_manipulator_set_speed(self->manipulator, speed);
    self->jpi_update = TRUE;
    self->jpd_update = TRUE;
    self->jq_update = TRUE;
    self->jqd_update = TRUE;
    self->ma_update = TRUE;
    self->mp_update = TRUE;
    self->ia_update = TRUE;
    self->mm_update = TRUE;
    self->mc_update = TRUE;
    self->mi_update = TRUE;
    self->mt_update = TRUE;
}

DVector*
d_dynamic_model_get_force (DDynamicModel    *self)
{
    return self->force;
}

void
d_dynamic_model_set_force (DDynamicModel    *self,
                           DVector          *force)
{
    if (self->force) {
        g_object_unref(self->force);
    }
    self->force = g_object_ref(force);
    self->mt_update = TRUE;
}

DVector*
d_dynamic_model_get_torque (DDynamicModel   *self)
{
    return d_manipulator_get_torque(self->manipulator);
}

void
d_dynamic_model_set_torque (DDynamicModel   *self,
                            DVector         *torque)
{
    d_manipulator_set_torque(self->manipulator, torque);
    self->mt_update = TRUE;
}

DVector*
d_dynamic_model_get_gravity (DDynamicModel  *self)
{
    return self->gravity;
}

void
d_dynamic_model_set_gravity (DDynamicModel  *self,
                             DVector        *gravity)
{
    if (self->gravity) {
        g_object_unref(self->gravity);
    }
    self->gravity = g_object_ref(gravity);
}

void
d_dynamic_model_solve_inverse (DDynamicModel    *self,
                               gdouble          interval)
{
    gsl_odeiv2_system sys = {
        d_dynamic_model_equation,
        NULL,
        6,
        self
    };
    gsl_odeiv2_driver *driver = gsl_odeiv2_driver_alloc_y_new(&sys,
                                        gsl_odeiv2_step_rk4,
                                        1e-6, 1e-6, 0.0);
    gdouble t = 0.0;
    gdouble y[6] = {
        d_vector_get(d_dynamic_model_get_axes(self), 0),
        d_vector_get(d_dynamic_model_get_axes(self), 1),
        d_vector_get(d_dynamic_model_get_axes(self), 2),
        d_vector_get(d_dynamic_model_get_speed(self), 0),
        d_vector_get(d_dynamic_model_get_speed(self), 1),
        d_vector_get(d_dynamic_model_get_speed(self), 2)
    };
    int s = gsl_odeiv2_driver_apply(driver, &t, interval, y);
    if (s != GSL_SUCCESS) {
        g_warning("driver returned %d\n", s);
        //TODO: Error handling
    }
    gsl_odeiv2_driver_free(driver);
}

void
d_dynamic_model_apply_force (DDynamicModel  *self,
                             DVector        *force,
                             gdouble        interval)
{
    d_dynamic_model_set_force (self, force);
    d_dynamic_model_solve_inverse(self, interval);
}

void
d_dynamic_model_apply_torque (DDynamicModel *self,
                              DVector       *torque,
                              gdouble       interval)
{
    d_dynamic_model_set_torque (self, torque);
    d_dynamic_model_solve_inverse(self, interval);
}
