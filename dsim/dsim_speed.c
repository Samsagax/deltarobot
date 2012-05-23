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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURSPEEDE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PROJECTNAME. If not, see <http://www.gnu.org/licenses/>.
 */

#include "dsim_speed.h"

/* GType register */
G_DEFINE_TYPE (DSpeed, d_speed, D_TYPE_VECTOR3);

/* Methods */
void
d_speed_to_string ( DSpeed *self, GString *string )
{
    d_vector3_to_string(D_VECTOR3(self), string);
}

/* Virtual Methods */
gdouble
d_speed_get (DSpeed* self, gint i)
{
    return D_VECTOR3_CLASS(d_speed_parent_class)->get(D_VECTOR3(self), i);
}

/* Create new DSpeed instance */
DSpeed*
d_speed_new ( gdouble s1,
              gdouble s2,
              gdouble s3 )
{
    DSpeed* a = D_SPEED (g_object_new (D_TYPE_SPEED, NULL));
    a->parent_instance.data[0] = s1;
    a->parent_instance.data[1] = s2;
    a->parent_instance.data[2] = s3;
    return a;
}

/* Dipose and finalize functions */
static void
d_speed_dispose (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_speed_parent_class)->dispose(gobject);
}

static void
d_speed_finalize (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_speed_parent_class)->finalize(gobject);
}

/* Initialization functions */
static void
d_speed_init (DSpeed* self)
{
    self->parent_instance.data[0] = 0.0;
    self->parent_instance.data[1] = 0.0;
    self->parent_instance.data[2] = 0.0;
}

static void
d_speed_class_init (DSpeedClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_speed_dispose;
    gobject_class->finalize = d_speed_finalize;
}

