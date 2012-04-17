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
 * dsim_error.h :
 */


#ifndef  DSIM_ERROR_INC
#define  DSIM_ERROR_INC

#include <dsim/dsim_common.h>

BEGIN_C_DECLS

extern const char *program_name;
extern void set_program_name    (const char *argv0);

extern void dsim_warning        (const char *msg);
extern void dsim_error          (const char *msg);
extern void dsim_fatal          (const char *msg);

END_C_DECLS

#endif   /* ----- #ifndef DSIM_ERROR_INC  ----- */
