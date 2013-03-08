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
 * dsim_trajectory_command.c : Message passing protocol for DTRajectoryControl
 */

#include "dsim_trajectory.h"

/* Forward declarations */
static void     d_trajectory_command_class_init (DTrajectoryCommandClass    *klass);

static void     d_trajectory_command_init       (DTrajectoryCommand         *self);

static void     d_trajectory_command_dispose    (GObject                    *obj);

static void     d_trajectory_command_finalize   (GObject                    *obj);

/* Implementation internals */
G_DEFINE_TYPE(DTrajectoryCommand, d_trajectory_command, G_TYPE_OBJECT);

static void
d_trajectory_command_class_init (DTrajectoryCommandClass    *klass)
{
    GObjectClass *obj_class = G_OBJECT_CLASS(klass);

    obj_class->dispose = d_trajectory_command_dispose;
    obj_class->finalize = d_trajectory_command_finalize;
}

static void
d_trajectory_command_init (DTrajectoryCommand   *self)
{
    self->command_type = OT_WAIT;
    self->data = NULL;
}

static void
d_trajectory_command_dispose (GObject   *obj)
{
    g_warning("d_trajectory_command_dispose is a stub");
    DTrajectoryCommand *self = D_TRAJECTORY_COMMAND(obj);

    switch (self->command_type) {
        case OT_MOVEL:
            if (self->data) {
                gsl_vector_free(self->data);
                self->data = NULL;
            }
        case OT_MOVEJ:
            if (self->data) {
                gsl_vector_free(self->data);
                self->data = NULL;
            }
            break;
        case OT_END:
            break;
        default:
            g_warning("cannot handle DCommandType %i", self->command_type);
            break;
    }
    G_OBJECT_CLASS(d_trajectory_command_parent_class)->dispose(obj);
}

static void
d_trajectory_command_finalize (GObject  *obj)
{
    G_OBJECT_CLASS(d_trajectory_command_parent_class)->finalize(obj);
}


/* Public API */
DTrajectoryCommand*
d_trajectory_command_new (DCommandType  cmdt,
                          gpointer      data)
{
    DTrajectoryCommand *dtc;
    dtc = g_object_new(D_TYPE_TRAJECTORY_COMMAND, NULL);
    dtc->command_type = cmdt;
    if (data) {
        switch (cmdt) {
            case OT_MOVEL:
                dtc->data = gsl_vector_alloc(3);
                gsl_vector_memcpy(dtc->data, data);
                break;
            case OT_MOVEJ:
                dtc->data = gsl_vector_alloc(3);
                gsl_vector_memcpy(dtc->data, data);
                break;
            case OT_END:
                break;
        }
    }
    return dtc;
}
