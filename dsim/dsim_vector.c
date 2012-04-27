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
 * dsim_vector.c :
 */

#include "dsim_vector.h"
#include <stdio.h>

/* GType register */
G_DEFINE_TYPE (DVector3, d_vector3, G_TYPE_OBJECT);

/* Virtual methods */
gdouble
d_vector3_get (DVector3 *self, gint index)
{
    //TODO: Make it lenght-safe
    return self->data[index];
}

void
d_vector3_set (DVector3 *self, gint index, gdouble value)
{
    self->data[index] = value;
}

void
d_vector3_substract (DVector3 *self, DVector3 *a)
{
    int i;
    for(i = 0; i < self->length; i++) {
        self->data[i] = self->data[i] - a->data[i];
    }
}

void
d_vector3_add (DVector3 *self, DVector3 *a)
{
    int i;
    for(i = 0; i < self->length; i++) {
        self->data[i] = self->data[i] - a->data[i];
    }
}


/* Create new DVector3 instance */
DVector3*
d_vector3_new()
{
    return D_VECTOR3(g_object_new(D_TYPE_VECTOR3, NULL));
}

/* Dispose and finalize functions */
static void
d_vector3_dispose (GObject *gobject)
{
    printf("Vector3 disposed\n");
    DVector3 *self = D_VECTOR3(gobject);
    /* Free allocated resources */
    if (self->data) {
        g_free(self->data);
        self->data = NULL;
    }
    /* Chain up */
    G_OBJECT_CLASS(d_vector3_parent_class)->dispose(gobject);
}

static void
d_vector3_finalize (GObject *gobject)
{
    /* Chain up */
    G_OBJECT_CLASS(d_vector3_parent_class)->finalize(gobject);
}

/* Initializer functions */
static void
d_vector3_class_init(DVector3Class* klass)
{
    printf("DVector3 class init\n");
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_vector3_dispose;
    gobject_class->finalize = d_vector3_finalize;
    klass->add = d_vector3_add;
    klass->substract = d_vector3_substract;
    klass->set = d_vector3_set;
    klass->get = d_vector3_get;
}

static void
d_vector3_init (DVector3 *self)
{
    printf("DVector3 instance init\n");
    self->length = 3;
    if (!(self->data)) {
        self->data = g_malloc0((self->length)*sizeof(gdouble));
    }
}
