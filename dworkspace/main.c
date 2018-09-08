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
static gdouble a = 29.3;
static gdouble b = 64.5;
static gdouble h = 3.8;
static gdouble r = 10.0;
static gdouble t_min = 0.0;
static gdouble t_max = 90.0;
static gdouble t_increment = 5.0;


static GOptionEntry entries[] =
{
      { "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "be verbose", NULL  },
      { "t-min", NULL, 0, G_OPTION_ARG_DOUBLE, &t_min, "minimum value of arm angle to test", NULL },
      { "t-max", NULL, 0, G_OPTION_ARG_DOUBLE, &t_max, "maximum value of arm angle to test", NULL },
      { "t-increment", NULL, 0, G_OPTION_ARG_DOUBLE, &t_increment, "increment value to test", NULL },
      { "near-arm", 'a', 0, G_OPTION_ARG_DOUBLE, &a, "value of 'a' length in robot", "A" },
      { "far-arm", 'b', 0, G_OPTION_ARG_DOUBLE, &b, "value of 'b' length in robot", "B" },
      { "moving-plt", 'h', 0, G_OPTION_ARG_DOUBLE, &h, "value of 'h' length in robot", "H" },
      { "fix-plt", 'r', 0, G_OPTION_ARG_DOUBLE, &r, "value of 'r' length in robot", "R" },
      { NULL  }
};

/*
 * Main function
 */
int
main(int argc, char* argv[])
{
    GError *parse_error = NULL;
    GOptionContext *context;

    context = g_option_context_new ("- generate non uniform grid of cartesian points in the robot workspace");
    g_option_context_add_main_entries (context, entries, NULL);
    if (!g_option_context_parse(context, &argc, &argv, &parse_error))
    {
        g_print("Options parsing failed: %s\n", parse_error->message);
        g_option_context_free(context);
        exit(1);
    }
    g_option_context_free(context);

    g_print("Using values interval [ %f , %f ], increment: %f \n", t_min, t_max, t_increment);

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

    gdouble t1 = t_min;
    gdouble t2 = t_min;
    gdouble t3 = t_min;

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
                    g_clear_error(&err);
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
