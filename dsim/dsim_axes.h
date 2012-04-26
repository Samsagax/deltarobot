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

#ifndef DAXES_H
#define DAXES_H

#include <glib-object.h>

/* Type macros */
#define D_TYPE_AXES             (d_axes_get_type ())
#define D_AXES(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), D_TYPE_AXES, DAxes))
#define D_IS_AXES(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), D_TYPE_AXES))
#define D_AXES_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), D_TYPE_AXES, DAxesClass))
#define D_IS_AXES_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), D_TYPE_AXES))
#define D_AXES_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), D_TYPE_AXES, DAxesClass))

/* Instance Structure of DAxes */
typedef struct _DAxes DAxes;
struct _DAxes {
    GObject         parent_instance;
    /* instance members */
    gdouble         axis[3];
};

/* Class Structure of DAxes */
typedef struct _DAxesClass DAxesClass;
struct _DAxesClass {
    GObjectClass      parent_class;
};

/* Returns GType associated with this object type */
GType   d_axes_get_type (void);

/* Methods */
DAxes*  d_axes_new          (void);

gdouble d_axes_get_axis     (DAxes *self, gint index);
void    d_axes_set_axis     (DAxes *self, gint index, gdouble value);

DAxes*  d_axes_substract    (DAxes *a, DAxes *b);
void    d_axes_to_string    (DAxes *self, gchar* string, int n);

#endif
