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

#ifndef  DSIM_COMMON_INC
#define  DSIM_COMMON_INC

#ifdef  __CPLUSPLUS
    #define BEGIN_C_DECLS   extern "C" {
    #define END_C_DECLS     }
#else      /* -----  not __CPLUSPLUS  ----- */
#   define BEGIN_C_DECLS
#   define END_C_DECLS
#endif     /* -----  __CPLUSPLUS  ----- */

#include <stdio.h>
#include <sys/types.h>


#if  STDC_HEADERS
#   include <stdlib.h>
#else      /* -----  not STDC_HEADERS  ----- */
#endif     /* -----  STDC_HEADERS  ----- */

#if HAVE_UNISTD_H
#  include <unistd.h>
#endif

#if  HAVE_ERRNO_H
#   include <errno.h>
#endif     /* -----  HAVE_ERRNO_H  ----- */
#ifndef errno
extern int errno;
#endif

#ifndef EXIT_SUCCESS
#  define EXIT_SUCCESS  0
#  define EXIT_FAILURE  1
#endif

#endif   /* ----- #ifndef DSIM_COMMON_INC  ----- */
