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
 * dsim_dynamic_spec.c :
 */

#include "dsim_dynamics.h"

/* Forward declarations */
static void         d_dynamic_spec_class_init   (DDynamicSpecClass  *klass);

static void         d_dynamic_spec_init         (DDynamicSpec       *self);

static void         d_dynamic_spec_dispose      (GObject            *obj);

static void         d_dynamic_spec_finalize     (GObject            *obj);

/* GType Register */
G_DEFINE_TYPE(DDynamicSpec, d_dynamic_spec, G_TYPE_OBJECT);

/* DDynamicSpec implementaion */
static void
d_dynamic_spec_init (DDynamicSpec   *self)
{
    self->low_arm_mass = 1.0;
    self->low_arm_moi = 1.0;
    self->upper_arm_mass = 1.0;
    self->upper_arm_moi = 1.0;
    self->platform_mass = 1.0;
}

static void
d_dynamic_spec_class_init (DDynamicSpecClass    *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_dynamic_spec_dispose;
    gobject_class->finalize = d_dynamic_spec_finalize;
}

static void
d_dynamic_spec_dispose (GObject *gobject)
{
    G_OBJECT_CLASS(d_dynamic_spec_parent_class)->dispose(gobject);
}

static void
d_dynamic_spec_finalize (GObject *gobject)
{
    G_OBJECT_CLASS (d_dynamic_spec_parent_class)->finalize (gobject);
}

/* Public API */
DDynamicSpec*
d_dynamic_spec_new (void)
{
    DDynamicSpec *ds = g_object_new(D_TYPE_DYNAMIC_SPEC, NULL);
    return ds;
}
