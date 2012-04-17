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
 * dsim_error.c :
 */

#include "dsim_common.h"
#include "dsim_error.h"

const char *program_name = NULL;

void
set_program_name (const char *path)
{
  if (!program_name)
    program_name = "prog_name";
}

static void error ( int exit_stat, const char *mode, const char *msg);

static void
error ( int exit_stat, const char *mode, const char *msg) {
    fprintf(stderr, "%s: %s: %s.\n", program_name, mode, msg);

    if ( exit_stat >= 0 )
        exit (exit_stat);
}

void
dsim_warning (const char *msg) {
    error (-1, "WARNING", msg);
}

void
dsim_error (const char *msg) {
    error (-1, "ERROR", msg);
}

void
dsim_fatal (const char *msg) {
    error (EXIT_FAILURE, "FATAL", msg);
}
