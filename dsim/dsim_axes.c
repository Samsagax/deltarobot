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

#include "dsim_axes.h"


/* ################ Simple DAxes ###################### */

/* Register Type */
G_DEFINE_TYPE (DAxes, d_axes, D_TYPE_VECTOR3);

/* Create new DAxes instance */
DAxes*
d_axes_new()
{
    return D_AXES(g_object_new(D_TYPE_AXES, NULL));
}

DAxes*
d_axes_new_full ( gdouble ax1,
                  gdouble ax2,
                  gdouble ax3 )
{
    DAxes* a = d_axes_new();
    d_axes_set(a, 0, ax1);
    d_axes_set(a, 1, ax2);
    d_axes_set(a, 2, ax3);
    return a;
}

/* Dispose and Finalize functions */
static void
d_axes_dispose (GObject *gobject)
{
    G_OBJECT_CLASS(d_axes_parent_class)->dispose(gobject);
}

static void
d_axes_finalize (GObject *gobject)
{
    G_OBJECT_CLASS (d_axes_parent_class)->finalize (gobject);
}

/* Init functions */
static void
d_axes_init ( DAxes* self )
{
    /* Stub Initialize function */
}

static void
d_axes_class_init ( DAxesClass *klass )
{
    /* Stub */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_axes_dispose;
    gobject_class->finalize = d_axes_finalize;
}

/* Methods */
gdouble
d_axes_get ( DAxes* self,
             gint   index )
{
    return D_VECTOR3_CLASS(d_axes_parent_class)->get(D_VECTOR3(self), index);
}

void
d_axes_set ( DAxes*     self,
             gint       index,
             gdouble    value )
{
    D_VECTOR3_CLASS(d_axes_parent_class)->set(D_VECTOR3(self), index, value);
}

void
d_axes_add ( DAxes* self,
             DAxes* a )
{
    D_VECTOR3_CLASS(d_axes_parent_class)->add(D_VECTOR3(self), D_VECTOR3(a));
}

void
d_axes_substract ( DAxes* self,
                   DAxes *a)
{
    D_VECTOR3_CLASS(d_axes_parent_class)->substract(D_VECTOR3(self), D_VECTOR3(a));
}

gchar*
d_axes_to_string ( DAxes* self )
{
    return d_vector3_to_string(D_VECTOR3(self));
}

/* ################ Extended DExtAxes ###################### */

/* Register Type */
G_DEFINE_TYPE (DExtAxes, d_ext_axes, G_TYPE_OBJECT);

/* Create new DAxes instance */
DExtAxes*
d_ext_axes_new()
{
    return D_EXTAXES(g_object_new(D_TYPE_EXTAXES, NULL));
}

/* Dispose and Finalize functions */
static void
d_ext_axes_dispose (GObject *gobject)
{
    DExtAxes *self = D_EXTAXES(gobject);
    /* Unref all */
    {
        int i;
        for (i = 0; i < 3; i++) {
            if (self->axes[i]) {
                g_object_unref(self->axes[i]);
                self->axes[i] = NULL;
            }
        }
    }
    /* Chain up */
    G_OBJECT_CLASS(d_ext_axes_parent_class)->dispose(gobject);
}

static void
d_ext_axes_finalize (GObject *gobject)
{
    G_OBJECT_CLASS (d_ext_axes_parent_class)->finalize (gobject);
}

/* Init functions */
static void
d_ext_axes_init ( DExtAxes* self )
{
    /* Stub initialize function */
    {
        int i;
        for (i = 0; i < 3; i++) {
            self->axes[i] = d_axes_new();
        }
    }
}

static void
d_ext_axes_class_init ( DExtAxesClass *klass )
{
    /* Stub */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_ext_axes_dispose;
    gobject_class->finalize = d_ext_axes_finalize;
}

/* Methods */
void
d_ext_axes_set ( DExtAxes   *self,
                 gint       i,
                 gint       j,
                 gdouble    value )
{
    d_axes_set(self->axes[i], j, value);
}

gdouble
d_ext_axes_get ( DExtAxes   *self,
                 gint       i,
                 gint       j )
{
    return d_axes_get(self->axes[i], j);
}
