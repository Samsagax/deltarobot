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
#include <math.h>

/* Forward declarations */
static void         d_dynamic_model_class_init      (DDynamicModelClass  *klass);

static void         d_dynamic_model_init            (DDynamicModel       *self);

static void         d_dynamic_model_dispose         (GObject            *obj);

static void         d_dynamic_model_finalize        (GObject            *obj);

static void         d_dynamic_model_set_geometry    (DDynamicModel  *self,
                                                     DGeometry      *geometry);

static void         d_dynamic_model_set_dynamic_spec(DDynamicModel  *self,
                                                     DDynamicSpec   *dynamic_spec);

/* GType Register */
G_DEFINE_TYPE(DDynamicModel, d_dynamic_model, G_TYPE_OBJECT);

/* DDynamicModel implementaion */
static void
d_dynamic_model_init (DDynamicModel   *self)
{
    self->geometry = NULL;
    self->dynamic_spec = NULL;
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

    if (self->geometry) {
        g_object_unref(self->geometry);
        self->geometry = NULL;
    }
    if (self->dynamic_spec) {
        g_object_unref(self->dynamic_spec);
        self->dynamic_spec = NULL;
    }

    /*  Chain Up */
    G_OBJECT_CLASS(d_dynamic_model_parent_class)->dispose(gobject);
}

static void
d_dynamic_model_finalize (GObject *gobject)
{
    G_OBJECT_CLASS (d_dynamic_model_parent_class)->finalize (gobject);
}

static void
d_dynamic_model_set_dynamic_spec (DDynamicModel *self,
                                  DDynamicSpec  *dynamic_spec)
{
    if (self->dynamic_spec) {
        g_object_unref(self->dynamic_spec);
    }
    self->dynamic_spec = g_object_ref(dynamic_spec);
}

static void
d_dynamic_model_set_geometry (DDynamicModel *self,
                              DGeometry     *geometry)
{
    if (self->geometry) {
        g_object_unref(self->geometry);
    }
    self->geometry = g_object_ref(geometry);
}

static DMatrix*
d_dynamic_model_get_inverse_jacobian_dt (DDynamicModel  *self,
                                         DVector        *pos,
                                         DVector        *axes,
                                         DVector        *speed_pos,
                                         DVector        *speed_axes)
{
    DMatrix *j_q_dot = d_matrix_new(3, 3);
    gdouble a = self->geometry->a;
    gdouble h = self->geometry->h;
    gdouble r = self->geometry->r;
    for (int i = 0; i < d_matrix_length(j_q_dot, 0); i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (double)i;
        gdouble t = d_vector_get(axes, i);
        gdouble t_dot = d_vector_get(speed_axes, i);
        d_matrix_set(j_q_dot, i, i,
                    2.0 * a * (
                        (d_vector_get(speed_pos, 0) * cos(phi)
                            + d_vector_get(speed_pos, 1) * sin(phi)
                            + d_vector_get(pos,2) * t_dot) * sin(t)
                         + ((d_vector_get(pos, 0) * cos(phi)
                            + d_vector_get(pos, 1) * sin(phi)
                            + h - r) * t_dot
                            - d_vector_get(speed_pos, 2)) * cos(t)));
    }

    return j_q_dot;
}

static DMatrix*
d_dynamic_model_get_inverse_jacobian (DDynamicModel *self,
                                      DVector       *pos,
                                      DVector       *axes)
{
    DMatrix *j_q = d_matrix_new(3, 3);
    gdouble a = self->geometry->a;
    gdouble h = self->geometry->h;
    gdouble r = self->geometry->r;
    for (int i = 0; i < d_matrix_length(j_q, 0); i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (double)i;
        gdouble t = d_vector_get(axes, i);
        d_matrix_set(j_q, i, i,
                    2.0 * a * ((d_vector_get(pos, 0) * cos(phi)
                                + d_vector_get(pos, 1) * sin(phi)
                                + h - r)
                                * sin(t)
                                - d_vector_get(pos, 2) * cos(t)));
    }

    return j_q;
}

static DMatrix*
d_dynamic_model_get_direct_jacobian_dt (DDynamicModel   *self,
                                        DVector         *axes,
                                        DVector         *speed_pos,
                                        DVector         *speed_axes)
{
    DMatrix *j_p_dot = d_matrix_new(3, 3);
    gdouble a = self->geometry->a;

    for (int i = 0; i < d_matrix_length(j_p_dot, 0); i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (double)i;
        gdouble t = d_vector_get(axes, i);
        gdouble t_dot = d_vector_get(speed_axes, i);

        gdouble gammax_dot = 2.0 * (d_vector_get(speed_pos, 0)
                                + a * cos(phi) * sin(t) * t_dot);
        gdouble gammay_dot = 2.0 * (d_vector_get(speed_pos, 1)
                                + a * sin(phi) * sin(t) * t_dot);
        gdouble gammaz_dot = 2.0 * (d_vector_get(speed_pos, 2)
                                - a * cos(t) * t_dot);

        d_matrix_set(j_p_dot, i, 0, - gammax_dot);
        d_matrix_set(j_p_dot, i, 1, - gammay_dot);
        d_matrix_set(j_p_dot, i, 2, - gammaz_dot);
    }

    return j_p_dot;
}

static DMatrix*
d_dynamic_model_get_direct_jacobian (DDynamicModel  *self,
                                     DVector        *pos,
                                     DVector        *axes)
{
    DMatrix *j_p = d_matrix_new(3, 3);
    gdouble a = self->geometry->a;
    gdouble h = self->geometry->h;
    gdouble r = self->geometry->r;
    for (int i = 0; i < d_matrix_length(j_p, 0); i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (double)i;
        gdouble t = d_vector_get(axes, i);
        gdouble gammax = 2.0 * (d_vector_get(pos, 0) + (h-r) * cos(phi)
                                - a * cos(phi) * cos(t));
        gdouble gammay = 2.0 * (d_vector_get(pos, 1) + (h-r) * sin(phi)
                                - a * sin(phi) * cos(t));
        gdouble gammaz = 2.0 * (d_vector_get(pos, 2) - a * sin(t));
        d_matrix_set(j_p, i, 0, - gammax);
        d_matrix_set(j_p, i, 1, - gammay);
        d_matrix_set(j_p, i, 2, - gammaz);
    }

    return j_p;
}

static DMatrix*
d_dynamic_model_get_theta_mass_matrix (DDynamicModel    *self,
                                       DVector          *axes)
{
    DMatrix *m_t = d_matrix_new(3, 3);
    for (int i = 0; i < d_matrix_length(m_t, 0); i++) {
        gdouble phi = G_PI * 120.0 / 180.0 * (double)i;
        gdouble t = d_vector_get(axes, i);
        d_matrix_set(m_t, i, 0, - cos(phi) * sin(t));
        d_matrix_set(m_t, i, 1, - sin(phi) * sin(t));
        d_matrix_set(m_t, i, 2, cos(t));
    }
    gdouble mass = (self->dynamic_spec->low_arm_mass / 2.0
                    + self->dynamic_spec->upper_arm_mass)
                    * self->geometry->a;
    d_matrix_scalar_mul(m_t, mass);

    return m_t;
}

static DMatrix*
d_dynamic_model_get_pos_mass (DDynamicModel     *self)
{
    DMatrix *m_p = d_matrix_new(3, 3);
    for (int i = 0; d_matrix_length(m_p, 0); i++) {
        gdouble mass = self->dynamic_spec->platform_mass
                        + 3.0 * self->dynamic_spec->upper_arm_mass;
        d_matrix_set(m_p, i, i, mass);
    }

    return m_p;
}

static DMatrix*
d_dynamic_model_get_theta_inertia (DDynamicModel     *self)
{
    DMatrix *m_p = d_matrix_new(3, 3);
    for (int i = 0; d_matrix_length(m_p, 0); i++) {
        gdouble mass = self->dynamic_spec->upper_arm_mass
                        * pow(self->geometry->a, 2.0)
                        + self->dynamic_spec->low_arm_moi;
        d_matrix_set(m_p, i, i, mass);
    }

    return m_p;
}

static DMatrix*
d_dynamic_model_get_model_inertia (DMatrix  *i_q,
                                   DMatrix  *m_p,
                                   DMatrix  *direct_jacobian,
                                   DMatrix  *inverse_jacobian)
{
    DMatrix *inverse_j_p = d_matrix_clone (direct_jacobian);
    d_matrix_inverse(inverse_j_p);

    DMatrix *ii = d_matrix_new(3, 3);

    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0,
                    inverse_jacobian->matrix, inverse_j_p->matrix,
                    0.0, ii->matrix);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    ii->matrix, m_p->matrix,
                    0.0, ii->matrix);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    ii->matrix, inverse_j_p->matrix,
                    0.0, ii->matrix);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    ii->matrix, inverse_jacobian->matrix,
                    0.0, ii->matrix);
    gsl_matrix_add(ii, i_q);

    g_object_unref(inverse_j_p);

    return ii;
}

static DMatrix*
d_dynamic_model_get_model_mass (DMatrix *m_q,
                                DMatrix *m_p,
                                DMatrix *j_p,
                                DMatrix *j_q)
{
    DMatrix *inverse_j_p = d_matrix_clone (j_p);
    d_matrix_inverse(inverse_j_p);

    DMatrix *mm = d_matrix_new(3, 3);

    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0,
                    j_q->matrix, inverse_j_p->matrix,
                    0.0, mm->matrix);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    mm->matrix, m_p->matrix,
                    0.0, mm->matrix);
    gsl_matrix_add(mm, m_q);

    g_object_unref(inverse_j_p);

    return mm;
}

static DMatrix*
d_dynamic_model_get_model_coriolis (DMatrix *m_q,
                                    DMatrix *m_p,
                                    DMatrix *j_p,
                                    DMatrix *j_p_dot,
                                    DMatrix *j_q,
                                    DMatrix *j_q_dot)
{
    DMatrix *inverse_j_p = d_matrix_clone(j_p);
    d_matrix_inverse(inverse_j_p);

    DMatrix *hh = d_matrix_new(3, 3);
    DMatrix *term1 = d_matrix_new(3, 3);
    DMatrix *term2 = d_matrix_new(3, 3);

    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    inverse_j_p->matrix, j_q_dot->matrix,
                    0.0, term_1->matrix);

    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    inverse_j_p->matrix, j_p_dot->matrix,
                    0.0, term2->matrix);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    term2->matrix, inverse_j_p->matrix,
                    0.0, term2->matrix);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    term2->matrix, j_q->matrix,
                    0.0, term2->matrix);

    gsl_matrix_add(term1->matrix, term2->matrix);

    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0,
                    j_q->matrix, inverse_j_p->matrix,
                    0.0, hh->matrix);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    hh->matrix, m_p->matrix,
                    0.0, hh->matrix);

    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0,
                    hh->matrix, term1->matrix,
                    0.0, hh->matrix);

    g_object_unref(inverse_j_p);
    g_object_unref(term1);
    g_object_unref(term2);

    return hh;
}

static DVector*
d_dynamic_model_get_model_torque (DMatrix   *j_q,
                                  DMAtrix   *j_p,
                                  DVector   *t_q,
                                  DVector   *q_p)
{
    DMatrix *inverse_j_p = d_matrix_clone(j_p);
    d_matrix_inverse(inverse_j_p);

    DMatrix *term = d_matrix_new(3, 3);

    gsl_blas_dgemm(CblasNoTrans, CblasTrans, 1.0,
                    j_q->matrix, inverse_j_p->matrix,
                    0.0, term->matrix);

    DVector *tt = d_matrix_vector_mul(term, q_p);
    d_vector_add(tt, t_q);

    return tt;
}

/* Public API */
DDynamicModel*
d_dynamic_model_new (DGeometry      *geometry,
                     DDynamicSpec   *dynamic_spec)
{
    DDynamicModel *dm = g_object_new(D_TYPE_DYNAMIC_MODEL, NULL);

    d_dynamic_model_set_geometry(dm, geometry);
    d_dynamic_model_set_dynamic_spec(dm, dynamic_spec);

    return dm;
}


