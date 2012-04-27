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

#include <stdio.h>
#include "dsim_pos.h"

/* GType register */
G_DEFINE_TYPE (DPos, d_pos, D_TYPE_VECTOR3);

/* Virtual Methods */
void
d_pos_set (DPos *self, gint i, gdouble value)
{
    D_VECTOR3_CLASS(d_pos_parent_class)->set(D_VECTOR3(self), i, value);
}

gdouble
d_pos_get (DPos* self, gint i)
{
    return D_VECTOR3_CLASS(d_pos_parent_class)->get(D_VECTOR3(self), i);
}

/* Create new DPos instance */
DPos*
d_pos_new (void)
{
    DPos* pos;
    pos = g_object_new(D_TYPE_POS, NULL);
    return pos;
}

/* Dispose and finalize functions */
static void
d_pos_dispose (GObject *gobject)
{
    printf("DPos being disposed\n");
    /* Chain up */
    G_OBJECT_CLASS(d_pos_parent_class)->dispose(gobject);
}

static void
d_pos_finalize (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_pos_parent_class)->finalize(gobject);
}

/* Initialization functions */
static void
d_pos_init (DPos* self)
{

}

static void
d_pos_class_init (DPosClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_pos_dispose;
    gobject_class->finalize = d_pos_finalize;
}

