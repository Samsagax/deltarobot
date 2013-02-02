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

#include "dsim_dynamics.h"
#include <gsl/gsl_permutation.h>

/* Forward declarations */
static void         d_dynamic_model_class_init      (DDynamicModelClass  *klass);

static void         d_dynamic_model_init            (DDynamicModel       *self);

static void         d_dynamic_model_dispose         (GObject            *obj);

static void         d_dynamic_model_finalize        (GObject            *obj);

static void         d_dynamic_model_set_geometry    (DDynamicModel  *self,
                                                     DGeometry      *geometry);

static void         d_dynamic_model_set_dynamic_spec(DDynamicModel  *self,
                                                     DDynamicSpec   *dynamic_spec);

static gsl_matrix*  d_dynamic_model_get_direct_jacobian_dt
                                                    (DDynamicModel  *self,
                                                     gsl_vector     *axes,
                                                     gsl_vector     *speed);

static gsl_matrix*  d_dynamic_model_get_direct_jacobian_inv
                                                    (DDynamicModel  *self,
                                                     gsl_vector     *axes);

static gsl_matrix*  d_dynamic_model_get_inverse_jacobian
                                                    (DDynamicModel  *self,
                                                     gsl_vector     *axes);

static gsl_matrix*  d_dynamic_model_get_inverse_jacobian_dt
                                                    (DDynamicModel  *self,
                                                     gsl_vector     *axes,
                                                     gsl_vector     *speed);

static gsl_matrix*  d_dynamic_model_get_inertia_axes(DDynamicModel  *self);

static gsl_matrix*  d_dynamic_model_get_mass_pos    (DDynamicModel  *self);

static gsl_matrix*  d_dynamic_model_get_mass_axes   (DDynamicModel  *self,
                                                     gsl_vector     *axes);

static gsl_matrix*  d_dynamic_model_get_model_inertia_inv
                                                    (DDynamicModel  *self,
                                                     gsl_vector     *axes);

static gsl_matrix*  d_dynamic_model_get_model_mass  (DDynamicModel  *self,
                                                     gsl_vector     *axes);

static gsl_matrix*  d_dynamic_model_get_model_coriolis
                                                    (DDynamicModel  *self,
                                                     gsl_vector     *axes,
                                                     gsl_vector     *speed);

static gsl_vector*  d_dynamic_model_get_model_torque(DDynamicModel  *self,
                                                     gsl_vector     *axes);

#define D_DYNAMIC_MODEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), D_TYPE_DYNAMIC_MODEL, DDynamicModelPrivate))
struct _DDynamicModelPrivate {
    /* Useful matrices */
    gsl_matrix  *jacobian_p_inv;
    gsl_matrix  *jacobian_p_dot;
    gsl_matrix  *jacobian_q;
    gsl_matrix  *jacobian_q_dot;

    gsl_matrix  *mass_axes;
    gsl_matrix  *mass_pos;
    gsl_matrix  *inertia_axes;

    /* Model matrices */
    gsl_matrix  *model_mass;
    gsl_matrix  *model_coriolis;
    gsl_matrix  *model_inertia_inv;
    gsl_vector  *model_torque;

    /* Update flags for some useful matrices */
    gboolean    jpi_update;
    gboolean    jpd_update;
    gboolean    jq_update;
    gboolean    jqd_update;

    gboolean    ma_update;
    gboolean    mp_update;
    gboolean    ia_update;

    /* Update flags for model matrices */
    gboolean    mm_update;
    gboolean    mc_update;
    gboolean    mi_update;
    gboolean    mt_update;
};

/* GType Register */
G_DEFINE_TYPE(DDynamicModel, d_dynamic_model, G_TYPE_OBJECT);

/* DDynamicModel implementaion */
static void
d_dynamic_model_init (DDynamicModel   *self)
{
    DDynamicModelPrivate    *priv;
    self->priv = priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    self->manipulator = NULL;
    self->force = gsl_vector_calloc(3);
    self->gravity = gsl_vector_calloc(3);

    priv->jacobian_p_inv = gsl_matrix_alloc(3, 3);
    priv->jacobian_p_dot = gsl_matrix_alloc(3, 3);
    priv->jacobian_q = gsl_matrix_alloc(3, 3);
    priv->jacobian_q_dot = gsl_matrix_alloc(3, 3);

    priv->jpd_update = TRUE;
    priv->jq_update = TRUE;
    priv->jpi_update = TRUE;
    priv->jqd_update = TRUE;

    priv->mass_axes = gsl_matrix_alloc(3, 3);
    priv->mass_pos = gsl_matrix_alloc(3, 3);
    priv->inertia_axes = gsl_matrix_alloc(3, 3);

    priv->ma_update = TRUE;
    priv->mp_update = TRUE;
    priv->ia_update = TRUE;

    priv->model_mass = gsl_matrix_alloc(3, 3);
    priv->model_coriolis = gsl_matrix_alloc(3, 3);
    priv->model_inertia_inv = gsl_matrix_alloc(3, 3);
    priv->model_torque = gsl_vector_alloc(3);

    priv->mm_update = TRUE;
    priv->mc_update = TRUE;
    priv->mi_update = TRUE;
    priv->mt_update = TRUE;
}

static void
d_dynamic_model_class_init (DDynamicModelClass    *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_dynamic_model_dispose;
    gobject_class->finalize = d_dynamic_model_finalize;

    g_type_class_add_private(klass, sizeof(DDynamicModelPrivate));
}

static void
d_dynamic_model_dispose (GObject *gobject)
{
    DDynamicModel *self = D_DYNAMIC_MODEL(gobject);
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (self->manipulator) {
        g_object_unref(self->manipulator);
        self->manipulator = NULL;
    }
    if (self->force) {
        gsl_vector_free(self->force);
        self->force = NULL;
    }
    if (self->gravity) {
        gsl_vector_free(self->gravity);
        self->gravity = NULL;
    }
    if (priv->jacobian_p_inv) {
        gsl_matrix_free(priv->jacobian_p_inv);
        priv->jacobian_p_inv = NULL;
    }
    if (priv->jacobian_p_dot) {
        gsl_matrix_free(priv->jacobian_p_dot);
        priv->jacobian_p_dot = NULL;
    }
    if (priv->jacobian_q) {
        gsl_matrix_free(priv->jacobian_q);
        priv->jacobian_q = NULL;
    }
    if (priv->jacobian_q_dot) {
        gsl_matrix_free(priv->jacobian_q_dot);
        priv->jacobian_q_dot = NULL;
    }
    if (priv->mass_axes) {
        gsl_matrix_free(priv->mass_axes);
        priv->mass_axes = NULL;
    }
    if (priv->mass_pos) {
        gsl_matrix_free(priv->mass_pos);
        priv->mass_pos = NULL;
    }
    if (priv->inertia_axes) {
        gsl_matrix_free(priv->inertia_axes);
        priv->inertia_axes = NULL;
    }
    if (priv->model_mass) {
        gsl_matrix_free(priv->model_mass);
        priv->model_mass = NULL;
    }
    if (priv->model_coriolis) {
        gsl_matrix_free(priv->model_coriolis);
        priv->model_coriolis = NULL;
    }
    if (priv->model_inertia_inv) {
        gsl_matrix_free(priv->model_inertia_inv);
        priv->model_inertia_inv = NULL;
    }
    if (priv->model_torque) {
        gsl_vector_free(priv->model_torque);
        priv->model_torque = NULL;
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
d_dynamic_model_update_jpi (DDynamicModel   *self,
                            gsl_vector      *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    gsl_vector *pos = gsl_vector_calloc(3);
    gsl_matrix *jpi = priv->jacobian_p_inv;
    DGeometry *geometry = self->manipulator->geometry;
    gdouble a = geometry->a;
    gdouble h = geometry->h;
    gdouble r = geometry->r;

    gsl_matrix_set_zero(jpi);
    d_solver_solve_direct(geometry, axes, pos);
    for (int i = 0; i < jpi->size1; i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (gdouble)i;
        gdouble t = gsl_vector_get(axes, i);
        gdouble gammax = 2.0 * (gsl_vector_get(pos, 0) + (h-r) * cos(phi)
                                - a * cos(phi) * cos(t));
        gdouble gammay = 2.0 * (gsl_vector_get(pos, 1) + (h-r) * sin(phi)
                                - a * sin(phi) * cos(t));
        gdouble gammaz = 2.0 * (gsl_vector_get(pos, 2) - a * sin(t));
        gsl_matrix_set(jpi, i, 0, - gammax);
        gsl_matrix_set(jpi, i, 1, - gammay);
        gsl_matrix_set(jpi, i, 2, - gammaz);
    }

    gsl_permutation *p = gsl_permutation_alloc(priv->jacobian_p_inv->size1);
    gsl_matrix *lu_decomp = gsl_matrix_alloc(priv->jacobian_p_inv->size1,
                                                priv->jacobian_p_inv->size2);
    gsl_matrix_memcpy(lu_decomp, priv->jacobian_p_inv);
    int sign;

    gsl_linalg_LU_decomp(lu_decomp, p, &sign);
    gsl_linalg_LU_invert(lu_decomp, p, priv->jacobian_p_inv);

    gsl_permutation_free(p);
    gsl_matrix_free(lu_decomp);

    priv->jpi_update = FALSE;
}

/*
 * Update direct jacobian derivative matrix with current parameters
 */
static void
d_dynamic_model_update_jpd (DDynamicModel   *self,
                            gsl_vector      *axes,
                            gsl_vector      *speed)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    gsl_vector *pos = gsl_vector_calloc(3);
    gsl_vector *speed_pos = gsl_vector_calloc(3);
    DGeometry *geometry = d_manipulator_get_geometry(self->manipulator);
    gsl_matrix *jp_inv = d_dynamic_model_get_direct_jacobian_inv(self, axes);
    gsl_matrix *jq = d_dynamic_model_get_inverse_jacobian(self, axes);
    gsl_matrix *jpd = priv->jacobian_p_dot;
    gdouble a = geometry->a;
    gsl_vector *temp = gsl_vector_calloc(3);

    gsl_matrix_set_zero(jpd);
    d_solver_solve_direct(geometry, axes, pos);
    gsl_blas_dgemv(CblasNoTrans, 1.0, jq,
                    speed, 0.0, temp);
    gsl_blas_dgemv(CblasNoTrans, 1.0, jp_inv,
                    temp, 0.0, speed_pos);
    for (int i = 0; i < jpd->size1; i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (double)i;
        gdouble t = gsl_vector_get(axes, i);
        gdouble t_dot = gsl_vector_get(speed, i);

        gdouble gammax_dot = 2.0 * (gsl_vector_get(speed_pos, 0)
                                + a * cos(phi) * sin(t) * t_dot);
        gdouble gammay_dot = 2.0 * (gsl_vector_get(speed_pos, 1)
                                + a * sin(phi) * sin(t) * t_dot);
        gdouble gammaz_dot = 2.0 * (gsl_vector_get(speed_pos, 2)
                                - a * cos(t) * t_dot);

        gsl_matrix_set(jpd, i, 0, - gammax_dot);
        gsl_matrix_set(jpd, i, 1, - gammay_dot);
        gsl_matrix_set(jpd, i, 2, - gammaz_dot);
    }

    priv->jpd_update = FALSE;

    gsl_vector_free(speed_pos);
    gsl_vector_free(pos);
    gsl_vector_free(temp);
}

/*
 * Update inverse jacobian matrix with current parameters
 */
static void
d_dynamic_model_update_jq (DDynamicModel    *self,
                           gsl_vector       *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    gsl_vector *pos = gsl_vector_calloc(3);
    gsl_matrix *jq = priv->jacobian_q;
    DGeometry* geometry = d_manipulator_get_geometry(self->manipulator);
    gdouble a = geometry->a;
    gdouble h = geometry->h;
    gdouble r = geometry->r;

    gsl_matrix_set_zero(jq);
    d_solver_solve_direct(geometry, axes, pos);
    for (int i = 0; i < jq->size1; i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (double)i;
        gdouble t = gsl_vector_get(axes, i);
        gsl_matrix_set(jq, i, i,
                        2.0 * a * ((gsl_vector_get(pos, 0) * cos(phi)
                                + gsl_vector_get(pos, 1) * sin(phi)
                                + h - r)
                                * sin(t)
                                - gsl_vector_get(pos, 2) * cos(t)));
    }

    priv->jq_update = FALSE;

    gsl_vector_free(pos);
}

/*
 * Update inverse jacobian derivative matrix with current parameters
 */
static void
d_dynamic_model_update_jqd (DDynamicModel   *self,
                            gsl_vector      *axes,
                            gsl_vector      *speed)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    gsl_vector *pos = gsl_vector_calloc(3);
    gsl_vector *speed_pos = gsl_vector_calloc(3);
    DGeometry *geometry = d_manipulator_get_geometry(self->manipulator);
    gsl_matrix *jp_inv = d_dynamic_model_get_direct_jacobian_inv(self, axes);
    gsl_matrix *jq = d_dynamic_model_get_inverse_jacobian(self, axes);

    gsl_matrix *jqd = priv->jacobian_q_dot;
    gdouble a = geometry->a;
    gdouble h = geometry->h;
    gdouble r = geometry->r;
    gsl_vector *temp = gsl_vector_calloc(3);

    gsl_matrix_set_zero(jqd);
    d_solver_solve_direct(geometry, axes, pos);
    gsl_blas_dgemv(CblasNoTrans, 1.0, jq,
                    speed, 0.0, temp);
    gsl_blas_dgemv(CblasNoTrans, 1.0, jp_inv,
                    temp, 0.0, speed_pos);
    for (int i = 0; i < jqd->size1 ; i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (gdouble)i;
        gdouble t = gsl_vector_get(axes, i);
        gdouble t_dot = gsl_vector_get(speed, i);
        gsl_matrix_set(jqd, i, i,
                        2.0 * a * (
                            (gsl_vector_get(speed_pos, 0) * cos(phi)
                                + gsl_vector_get(speed_pos, 1) * sin(phi)
                                + gsl_vector_get(pos,2) * t_dot) * sin(t)
                            + ((gsl_vector_get(pos, 0) * cos(phi)
                                + gsl_vector_get(pos, 1) * sin(phi)
                                + h - r) * t_dot
                                - gsl_vector_get(speed_pos, 2)) * cos(t)));
    }

    priv->jqd_update = FALSE;

    gsl_vector_free(speed_pos);
    gsl_vector_free(pos);
    gsl_vector_free(temp);
}

/*
 * Update axes mass matrix with current parameters
 */
static void
d_dynamic_model_update_mass_axes (DDynamicModel *self,
                                  gsl_vector    *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    gsl_matrix *ma = priv->mass_axes;

    gsl_matrix_set_zero(ma);
    for (int i = 0; i < ma->size1; i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (double)i;
        gdouble t = gsl_vector_get(axes, i);
        gsl_matrix_set(ma, i, 0, - cos(phi) * sin(t));
        gsl_matrix_set(ma, i, 1, - sin(phi) * sin(t));
        gsl_matrix_set(ma, i, 2, cos(t));
    }
    gdouble mass = (self->manipulator->dynamic_params->low_arm_mass / 2.0
                    + self->manipulator->dynamic_params->upper_arm_mass)
                    * self->manipulator->geometry->a;
    gsl_matrix_scale(ma, mass);

    priv->ma_update = FALSE;
}

/*
 * Update axes mass matrix with current parameters
 */
static void
d_dynamic_model_update_mass_pos (DDynamicModel  *self)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    gsl_matrix *mp = priv->mass_pos;

    gsl_matrix_set_zero(mp);
    for (int i = 0; i < mp->size1; i++) {
        gdouble mass = self->manipulator->dynamic_params->platform_mass
                        + 3.0 * self->manipulator->dynamic_params->upper_arm_mass;
        gsl_matrix_set(mp, i, i, mass);
    }

    priv->mp_update = FALSE;
}

/*
 * Update axes inertia matrix with current parameters
 */
static void
d_dynamic_model_update_inertia_axes (DDynamicModel  *self)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    gsl_matrix *ia = priv->inertia_axes;

    gsl_matrix_set_zero(ia);
    for (int i = 0; i < ia->size1; i++) {
        gdouble mass = self->manipulator->dynamic_params->upper_arm_mass
                        * pow(self->manipulator->geometry->a, 2.0)
                        + self->manipulator->dynamic_params->low_arm_moi;
        gsl_matrix_set(ia, i, i, mass);
    }

    priv->ia_update = FALSE;
}

/*
 * Update model mass matrix with current parameters
 */
static void
d_dynamic_model_update_model_mass (DDynamicModel    *self,
                                   gsl_vector       *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    gsl_matrix *mass = priv->model_mass;
    gsl_matrix *jp = d_dynamic_model_get_direct_jacobian_inv(self, axes);
    gsl_matrix *jq = d_dynamic_model_get_inverse_jacobian(self, axes);
    gsl_matrix *mp = d_dynamic_model_get_mass_pos(self);
    gsl_matrix *mq = d_dynamic_model_get_mass_axes(self, axes);
    gsl_matrix *temp = gsl_matrix_calloc(3, 3);

    gsl_matrix_set_all(mass, 0);

    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0,
                    jq, jp, 0.0, temp);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    temp, mp, 0.0, mass);
    gsl_matrix_add(mass, mq);

    priv->mm_update = FALSE;

    gsl_matrix_free(temp);
}

/*
 * Update model inertia matrix with current parameters
 */
static void
d_dynamic_model_update_model_inertia_inv (DDynamicModel *self,
                                          gsl_vector    *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    gsl_matrix *inertia = priv->model_inertia_inv;
    gsl_matrix *jp = d_dynamic_model_get_direct_jacobian_inv(self, axes);
    gsl_matrix *jq = d_dynamic_model_get_inverse_jacobian(self, axes);
    gsl_matrix *mp = d_dynamic_model_get_mass_pos(self);
    gsl_matrix *iq = d_dynamic_model_get_inertia_axes(self);
    gsl_matrix *temp = gsl_matrix_calloc(3, 3);

    gsl_matrix_set_zero(inertia);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0,
                    jq, jp,
                    0.0, temp);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    temp, mp,
                    0.0, inertia);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    inertia, jp,
                    0.0, temp);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    temp, jq,
                    0.0, inertia);
    gsl_matrix_add(inertia, iq);

    gsl_permutation *p = gsl_permutation_alloc(priv->jacobian_p_inv->size1);
    gsl_matrix *lu_decomp = gsl_matrix_alloc(priv->model_inertia_inv->size1,
                                                priv->model_inertia_inv->size2);
    gsl_matrix_memcpy(lu_decomp, priv->model_inertia_inv);
    int sign;
    gsl_linalg_LU_decomp(lu_decomp, p, &sign);
    gsl_linalg_LU_invert(lu_decomp, p, priv->model_inertia_inv);

    gsl_permutation_free(p);
    gsl_matrix_free(lu_decomp);

    priv->mi_update = FALSE;

    gsl_matrix_free(temp);
}

/*
 * Update model coriolis matrix with current parameters
 */
static void
d_dynamic_model_update_model_coriolis (DDynamicModel    *self,
                                       gsl_vector       *axes,
                                       gsl_vector       *speed)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    gsl_matrix *coriolis = priv->model_coriolis;
    gsl_matrix *jp = d_dynamic_model_get_direct_jacobian_inv(self, axes);
    gsl_matrix *jq = d_dynamic_model_get_inverse_jacobian(self, axes);
    gsl_matrix *jpd = d_dynamic_model_get_direct_jacobian_dt(self, axes, speed);
    gsl_matrix *jqd = d_dynamic_model_get_inverse_jacobian_dt(self, axes, speed);
    gsl_matrix *mp = d_dynamic_model_get_mass_pos(self);
    gsl_matrix *term1 = gsl_matrix_calloc(3, 3);
    gsl_matrix *term2 = gsl_matrix_calloc(3, 3);
    gsl_matrix *temp = gsl_matrix_calloc(3, 3);

    gsl_matrix_set_zero(coriolis);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    jp, jqd,
                    0.0, term1);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    jp, jpd,
                    0.0, term2);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    term2, jp,
                    0.0, temp);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    temp, jq,
                    0.0, term2);
    gsl_matrix_add(term1, term2);
    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0,
                    jq, jp,
                    0.0, coriolis);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    coriolis, mp,
                    0.0, temp);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    temp, term1,
                    0.0, coriolis);

    gsl_matrix_free(term1);
    gsl_matrix_free(term2);
    gsl_matrix_free(temp);

    priv->mc_update = FALSE;
}

/*
 * Update model torque vector with current parameters
 */
static void
d_dynamic_model_update_model_torque (DDynamicModel  *self,
                                     gsl_vector     *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    gsl_vector *torque = priv->model_torque;
    gsl_vector *ff = self->force;
    gsl_vector *tt = d_manipulator_get_torque(self->manipulator);
    gsl_matrix *jp = d_dynamic_model_get_direct_jacobian_inv(self, axes);
    gsl_matrix *jq = d_dynamic_model_get_inverse_jacobian(self, axes);
    gsl_vector *temp = gsl_vector_calloc(3);

    gsl_vector_set_zero(torque);
    gsl_blas_dgemv(CblasTrans, 1.0,
                    jp, ff, 0.0, temp);
    gsl_blas_dgemv(CblasNoTrans, 1.0,
                    jq, temp, 0.0, torque);
    gsl_vector_add(torque, tt);

    priv->mt_update = FALSE;

    gsl_vector_free(temp);
}

static gsl_matrix*
d_dynamic_model_get_inverse_jacobian_dt (DDynamicModel  *self,
                                         gsl_vector     *axes,
                                         gsl_vector     *speed)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (priv->jqd_update) {
        d_dynamic_model_update_jqd(self, axes, speed);
    }
    return priv->jacobian_q_dot;
}

static gsl_matrix*
d_dynamic_model_get_inverse_jacobian (DDynamicModel *self,
                                      gsl_vector    *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (priv->jq_update) {
        d_dynamic_model_update_jq(self, axes);
    }
    return priv->jacobian_q;
}

static gsl_matrix*
d_dynamic_model_get_direct_jacobian_dt (DDynamicModel   *self,
                                        gsl_vector      *axes,
                                        gsl_vector      *speed)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (priv->jpd_update) {
        d_dynamic_model_update_jpd(self, axes, speed);
    }
    return priv->jacobian_p_dot;
}

static gsl_matrix*
d_dynamic_model_get_direct_jacobian_inv (DDynamicModel  *self,
                                         gsl_vector     *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (priv->jpi_update) {
        d_dynamic_model_update_jpi(self, axes);
    }
    return priv->jacobian_p_inv;
}

static gsl_matrix*
d_dynamic_model_get_inertia_axes (DDynamicModel *self)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (priv->ia_update) {
        d_dynamic_model_update_inertia_axes(self);
    }

    return priv->inertia_axes;
}

static gsl_matrix*
d_dynamic_model_get_mass_pos (DDynamicModel     *self)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (priv->mp_update) {
        d_dynamic_model_update_mass_pos(self);
    }

    return priv->mass_pos;
}

static gsl_matrix*
d_dynamic_model_get_mass_axes (DDynamicModel    *self,
                               gsl_vector       *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (priv->ma_update) {
        d_dynamic_model_update_mass_axes(self, axes);
    }

    return priv->mass_axes;
}

static gsl_matrix*
d_dynamic_model_get_model_inertia_inv (DDynamicModel    *self,
                                       gsl_vector       *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (priv->mi_update) {
        d_dynamic_model_update_model_inertia_inv(self, axes);
    }

    return priv->model_inertia_inv;
}

static gsl_matrix*
d_dynamic_model_get_model_mass (DDynamicModel   *self,
                                gsl_vector      *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (priv->mm_update) {
        d_dynamic_model_update_model_mass(self, axes);
    }

    return priv->model_mass;
}

static gsl_matrix*
d_dynamic_model_get_model_coriolis (DDynamicModel   *self,
                                    gsl_vector      *axes,
                                    gsl_vector      *speed)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (priv->mc_update) {
        d_dynamic_model_update_model_coriolis(self, axes, speed);
    }

    return priv->model_coriolis;
}

static gsl_vector*
d_dynamic_model_get_model_torque (DDynamicModel *self,
                                  gsl_vector    *axes)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    if (priv->mt_update) {
        d_dynamic_model_update_model_torque(self, axes);
    }

    return priv->model_torque;
}

static void
d_dynamic_model_matrices_outdated(DDynamicModel *self)
{
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(self);

    priv->jpi_update = TRUE;
    priv->jpd_update = TRUE;
    priv->jq_update = TRUE;
    priv->jqd_update = TRUE;
    priv->ma_update = TRUE;
    priv->mp_update = TRUE;
    priv->ia_update = TRUE;
    priv->mm_update = TRUE;
    priv->mc_update = TRUE;
    priv->mi_update = TRUE;
    priv->mt_update = TRUE;
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
    DDynamicModelPrivate *priv = D_DYNAMIC_MODEL_GET_PRIVATE(model);

    /* Matrices holding the coefficients on the model differential equation */
    gsl_matrix *mi, *mh, *mm;
    gsl_vector *mt;

    /* Speed and positions in axes space */
//    gsl_vector_const_view q, q_dot;
    gsl_vector_view q_dot_dot, q_dot_2;

    /* Fill the axes position and speed, request an update in the model */
    //Remove gsl_vectors, use gsl_vector_views
    //TODO: silent const warnings
    gsl_vector_const_view q = gsl_vector_const_view_array(&(y[0]), 3);
    gsl_vector_const_view q_dot = gsl_vector_const_view_array(&(y[3]), 3);
    //d_dynamic_model_set_axes(model, q);
    //d_dynamic_model_set_speed(model, q_dot);

    d_dynamic_model_matrices_outdated(model);
    /* Fill model matrices */
    mt = d_dynamic_model_get_model_torque(model, &q.vector);
    mi = d_dynamic_model_get_model_inertia_inv(model, &q.vector);
    mh = d_dynamic_model_get_model_coriolis(model, &q.vector, &q_dot.vector);
    mm = d_dynamic_model_get_model_mass(model, &q.vector);

    /* Calculate acceleration */
    /* d²q/dt² = I⁻¹ ( T - H * dq/dt + M * g ) */
    /* Set the dydt array */
    q_dot_2 = gsl_vector_view_array(&(dydt[0]), 3);
    gsl_vector_memcpy(&q_dot_2.vector, &q_dot.vector);
    q_dot_dot = gsl_vector_view_array(&(dydt[3]), 3);
    gsl_vector *v_g = model->gravity;
    gsl_vector *q_dot_dot_temp = gsl_vector_calloc(3);

    gsl_blas_dgemv(CblasNoTrans, 1.0, mm, v_g, 0.0, q_dot_dot_temp);
    gsl_blas_dgemv(CblasNoTrans, -1.0, mh, &q_dot.vector, 1.0, q_dot_dot_temp);
    gsl_vector_add(q_dot_dot_temp, mt);
    gsl_blas_dgemv(CblasNoTrans, 1.0, mi, q_dot_dot_temp, 0.0, &q_dot_dot.vector);

//    /* Set the dydt array */
//    dydt[0] = gsl_vector_get(q_dot, 0);
//    dydt[1] = gsl_vector_get(q_dot, 1);
//    dydt[2] = gsl_vector_get(q_dot, 2);
//    dydt[3] = gsl_vector_get(q_dot_dot, 0);
//    dydt[4] = gsl_vector_get(q_dot_dot, 1);
//    dydt[5] = gsl_vector_get(q_dot_dot, 2);

    /* Free memory */
    gsl_vector_free(q_dot_dot_temp);

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

gsl_vector*
d_dynamic_model_get_axes (DDynamicModel *self)
{
    return d_manipulator_get_axes(self->manipulator);
}

void
d_dynamic_model_set_axes (DDynamicModel *self,
                          gsl_vector    *axes)
{
    d_manipulator_set_axes(self->manipulator, axes);
}

gsl_vector*
d_dynamic_model_get_speed (DDynamicModel    *self)
{
    return d_manipulator_get_speed(self->manipulator);
}

void
d_dynamic_model_set_speed (DDynamicModel    *self,
                           gsl_vector       *speed)
{
    d_manipulator_set_speed(self->manipulator, speed);
}

gsl_vector*
d_dynamic_model_get_force (DDynamicModel    *self)
{
    return self->force;
}

void
d_dynamic_model_set_force (DDynamicModel    *self,
                           gsl_vector       *force)
{
    gsl_vector_memcpy(self->force, force);
}

gsl_vector*
d_dynamic_model_get_torque (DDynamicModel   *self)
{
    return d_manipulator_get_torque(self->manipulator);
}

void
d_dynamic_model_set_torque (DDynamicModel   *self,
                            gsl_vector      *torque)
{
    d_manipulator_set_torque(self->manipulator, torque);
}

gsl_vector*
d_dynamic_model_get_gravity (DDynamicModel  *self)
{
    return self->gravity;
}

void
d_dynamic_model_set_gravity (DDynamicModel  *self,
                             gsl_vector     *gravity)
{
    gsl_vector_memcpy(self->gravity, gravity);
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
        gsl_vector_get(d_dynamic_model_get_axes(self), 0),
        gsl_vector_get(d_dynamic_model_get_axes(self), 1),
        gsl_vector_get(d_dynamic_model_get_axes(self), 2),
        gsl_vector_get(d_dynamic_model_get_speed(self), 0),
        gsl_vector_get(d_dynamic_model_get_speed(self), 1),
        gsl_vector_get(d_dynamic_model_get_speed(self), 2)
    };
    int s = gsl_odeiv2_driver_apply(driver, &t, interval, y);
    if (s != GSL_SUCCESS) {
        g_warning("driver returned %d\n", s);
        //TODO: Error handling
    }
    gsl_vector_const_view q = gsl_vector_const_view_array(&(y[0]), 3);
    gsl_vector_const_view q_dot = gsl_vector_const_view_array(&(y[3]), 3);
    d_dynamic_model_set_axes(self, &q.vector);
    d_dynamic_model_set_speed(self, &q_dot.vector);
    gsl_odeiv2_driver_free(driver);
}
