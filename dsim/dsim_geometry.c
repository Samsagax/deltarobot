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
 * dsim_geometry.c :
 */

#include "dsim_geometry.h"

/* GType Register */
G_DEFINE_TYPE(DGeometry, d_geometry, G_TYPE_OBJECT);

/* Create new DGeometry object */
DGeometry*
d_geometry_new ( gdouble a,
                 gdouble b,
                 gdouble h,
                 gdouble r )
{
    DGeometry *g = D_GEOMETRY(g_object_new(D_TYPE_GEOMETRY, NULL));
    g->a = a;
    g->b = b;
    g->h = h;
    g->r = r;
    return g;
}

/* Dispose and Finalize functions */
static void
d_geometry_dispose (GObject *gobject)
{
    G_OBJECT_CLASS(d_geometry_parent_class)->dispose(gobject);
}

static void
d_geometry_finalize (GObject *gobject)
{
    G_OBJECT_CLASS (d_geometry_parent_class)->finalize (gobject);
}

/* Init functions */
static void
d_geometry_init ( DGeometry* self ) {}

static void
d_geometry_class_init ( DGeometryClass *klass )
{
    /* Stub */
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    gobject_class->dispose = d_geometry_dispose;
    gobject_class->finalize = d_geometry_finalize;
}
