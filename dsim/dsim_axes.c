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
#include <stdio.h>

/* Declares *_init functions */
G_DEFINE_TYPE (DAxes , d_axes, D_TYPE_VECTOR3);

/* Create new DAxes instance */
DAxes*
d_axes_new()
{
    return D_AXES(g_object_new(D_TYPE_AXES, NULL));
}

/* Dispose and Finalize functions */
static void
d_axes_dispose (GObject *gobject)
{
    DAxes *self = D_AXES (gobject);
    G_OBJECT_CLASS(d_axes_parent_class)->dispose(gobject);
}

static void
d_axes_finalize (GObject *gobject)
{
    DAxes *self = D_AXES(gobject);
    G_OBJECT_CLASS (d_axes_parent_class) ->finalize (gobject);
}

/* Init functions */
static void
d_axes_init ( DAxes *self ) {}

static void
d_axes_class_init ( DAxesClass *klass )
{
    /* Stub */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_axes_dispose;
    gobject_class->finalize = d_axes_finalize;
}

/* Methods */

void
d_axes_substract (DAxes* self, DAxes *a)
{
    return ;
    //TODO!!!
}

void
d_axes_to_string (DAxes *self, gchar* string, int n)
{
    g_snprintf(string, n, "Axis: %f; %f; %f",
                self->axis[0],
                self->axis[1],
                self->axis[2]);
}
