/*
 * Copyright (c) 2018, Joaquín Ignacio Aramendía
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
 * main.c :
 */

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include <dsim/dsim.h>


static gboolean axes = TRUE;
static gboolean cartesian = FALSE;
static gboolean verbose = FALSE;
static gdouble a = 50.0;
static gdouble b = 60.0;
static gdouble h = 20.0;
static gdouble r = 10.0;

static GOptionEntry entries[] =
{
      { "axes", 'x', 0, G_OPTION_ARG_NONE, &axes, "Use axes, default mode", NULL  },
      { "cartesian", 'c', 0, G_OPTION_ARG_NONE, &cartesian, "Use cartesian mode", NULL },
      { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL  },
      { 0, 'a', 0, G_OPTION_ARG_DOUBLE, &a, "value of 'a' length in robot", 'A' },
      { 0, 'b', 0, G_OPTION_ARG_DOUBLE, &b, "value of 'b' length in robot", 'B' },
      { 0, 'h', 0, G_OPTION_ARG_DOUBLE, &h, "value of 'h' length in robot", 'H' },
      { 0, 'r', 0, G_OPTION_ARG_DOUBLE, &r, "value of 'r' length in robot", 'R' },
      { NULL  }

};


static gdouble t_min = 0.0;
static gdouble t_max = 120.0;
static gdouble t_increment = 1;

/*
 * Main function
 */
int
main(int argc, char* argv[])
{
    GError *parse_error = NULL;
    GOptionContext *context;

    context = g_option_context_new ("<min> <max> <increment>-- test a range of input values against the workspace");
    g_option_context_add_main_entries (context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &parse_error))
    {
        g_print("Options parsing failed: %s\n", parse_error->message);
        exit(1);
    }

    if(argc >= 3)
    {
        t_min = g_strtod(argv[0], NULL);
        t_max = g_strtod(argv[1], NULL);
        t_increment = g_strtod(argv[2], NULL);
    }
    g_print("Using values interval [ %f , %f ], increment: %f \n", t_min, t_max, t_increment);

    gdouble t1 = t_min;
    gdouble t2 = t_min;
    gdouble t3 = t_min;

    DGeometry *geometry = d_geometry_new (a, b, h, r);
    g_print ( "Current geometry [ %f, %f, %f, %f ] \n", a, b, h, r );

    gsl_vector *pos = gsl_vector_alloc(3);
    gsl_vector *axes = gsl_vector_alloc(3);
    GError *err = NULL;

    GFile *out_file = g_file_new_for_path ("workspace.asc");
    GFileOutputStream *out_stream = g_file_replace(out_file,
            NULL,
            FALSE,
            G_FILE_CREATE_NONE,
            NULL,
            NULL);
    GString *buffer = g_string_new(NULL);

    while(t3 <= t_max)
    {
        while (t2 <= t_max)
        {
            while (t1 <= t_max)
            {
                if (verbose)
                {
                    g_print ( "Current axes [ %f, %f, %f ] \n", t1, t2, t3 );
                }
                gsl_vector_set(axes, 0, t1 / 180.0 * G_PI);
                gsl_vector_set(axes, 1, t2 / 180.0 * G_PI);
                gsl_vector_set(axes, 2, t3 / 180.0 * G_PI);
                d_solver_solve_direct (geometry, axes, pos, &err);
                if (err)
                {
                    err = NULL;
                } else {
                    if (verbose)
                    {
                        g_print ( "Point [ %f, %f, %f ] \n",
                                gsl_vector_get(pos, 0),
                                gsl_vector_get(pos, 1),
                                gsl_vector_get(pos, 2));
                    }
                    g_string_printf(buffer,
                            "%f %f %f\n",
                            gsl_vector_get(pos, 0),
                            gsl_vector_get(pos, 1),
                            gsl_vector_get(pos, 2));
                    g_output_stream_write (G_OUTPUT_STREAM (out_stream),
                            buffer->str,
                            buffer->len,
                            NULL,
                            NULL);
                }
                t1 += t_increment;
            }
            t1 = t_min;
            t2 += t_increment;
        }
        t1 = t_min;
        t2 = t_min;
        t3 += t_increment;
    }

    g_output_stream_close(out_stream, NULL, NULL);
    gsl_vector_free(axes);
    g_clear_object(&geometry);
    g_string_free(buffer, TRUE);

    return 0;
}
