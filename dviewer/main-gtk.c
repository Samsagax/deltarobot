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
 * main-gtk.c :
 */
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkgl.h>

#include <GL/gl.h>
#include <GL/glu.h>

static void
draw_init (GtkWidget    *widget,
           gpointer      data)
{
    GdkGLContext    *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable   *gldrawable = gtk_widget_get_gl_drawable(widget);

    if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext)){
        return;
    }
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    gdk_gl_drawable_gl_end(gldrawable);
}

static gboolean
draw ( GtkWidget        *widget,
       GdkEventExpose   *event,
       gpointer          data)
{
    GdkGLContext    *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable   *gldrawable = gtk_widget_get_gl_drawable(widget);

    /* OpenGL BEGIN */
    if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext)) {
        g_printf("Error\n");
        return FALSE;
    }

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    GLUquadricObj *pObj;
    pObj = gluNewQuadric();
    gluQuadricDrawStyle(pObj, GLU_FILL);
    gluQuadricNormals(pObj, GLU_SMOOTH);
    gluQuadricOrientation(pObj, GLU_OUTSIDE);
    glColor3f(0.7f, 0.2f, 0.2f);
    gluSphere(pObj, 1.0, 20, 10);
    glColor3f(0.5f, 0.5f, 0.5f);

    gluDeleteQuadric(pObj);
    glPopMatrix();

    if (gdk_gl_drawable_is_double_buffered (gldrawable)) {
        gdk_gl_drawable_swap_buffers (gldrawable);
    } else {
        glFlush();
    }
    gdk_gl_drawable_gl_end(gldrawable);
    /* OpenGL END */

    g_object_unref(G_OBJECT(glcontext));
    g_object_unref(G_OBJECT(gldrawable));
    return TRUE;
}

static gboolean
reshape (GtkWidget          *widget,
         GdkEventConfigure  *event,
         gpointer            data )
{
    GdkGLContext    *glcontext = gtk_widget_get_gl_context (widget);
    GdkGLDrawable   *gldrawable = gtk_widget_get_gl_drawable (widget);

    GLfloat h = (GLfloat) (widget->allocation.height) / (GLfloat) (widget->allocation.width);

    /* OpenGL BEGIN */
    if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext)) {
        return FALSE;
    }

    glViewport(0, 0, widget->allocation.width, widget->allocation.height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, h, 1.0, 400.0);
    glMatrixMode (GL_MODELVIEW);
    glTranslatef(0.0, 0.0, -50.0);

    gdk_gl_drawable_gl_end(gldrawable);
    /* OpenGL END */
}

static gboolean
key_handler ( GtkWidget     *widget,
              GdkEventKey   *event,
              gpointer       data )
{
    switch (event->keyval) {
        case GDK_KEY_Escape:
            gtk_main_quit();
            break;
        default:
            return FALSE;
    }

    gdk_window_invalidate_rect(widget->window, &widget->allocation, FALSE);
    return TRUE;
}

static GtkWidget*
create_main_window(void) {
    GtkWidget       *window;
    GtkWidget       *drawing_area;

    /* Print OpenGL version */
    gint minorGLVer, majorGLVer;
    gdk_gl_query_version (&majorGLVer, &minorGLVer);
    g_print("OpenGL extension version: %d.%d\n", majorGLVer, minorGLVer);

    /* Configure OpenGL visual */
    GdkGLConfig     *glconfig;
    glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB       |
                                          GDK_GL_MODE_DEPTH     |
                                          GDK_GL_MODE_DOUBLE    );
    if (glconfig == NULL) {
        g_print("*** Cannot get double-buffered visual.\n");
        g_print("*** Trying single-buffered visual.\n");

        glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB   |
                                              GDK_GL_MODE_DEPTH );
        if (glconfig == NULL) {
            g_print("*** No appropriate OpenGL-capable visual available.\n");
            return NULL;
        }
    }

    /* Set up drawing area */
    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_size_request (drawing_area, 400, 400);

    /* Set up drawing area OpenGL capabilities */
    gtk_widget_set_gl_capability ( drawing_area,
                                   glconfig,
                                   NULL,
                                   TRUE,
                                   GDK_GL_RGBA_TYPE);
//    g_object_unref(glconfig);

    /* Set up drawing area behaviour */
    gtk_widget_add_events ( drawing_area, GDK_VISIBILITY_NOTIFY_MASK );
    g_signal_connect_after  ( G_OBJECT(drawing_area),
                              "realize",
                              G_CALLBACK (draw_init),
                              NULL );
    g_signal_connect        ( G_OBJECT(drawing_area),
                              "configure_event",
                              G_CALLBACK(reshape),
                              NULL );
    g_signal_connect        ( G_OBJECT(drawing_area),
                              "expose_event",
                              G_CALLBACK(draw),
                              NULL );

    /* Create the window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(window), "DSim GL Viewer");
    gtk_window_set_icon_name(GTK_WINDOW(window), "applications-system");

    /* Set layout */
    gtk_container_add (GTK_CONTAINER (window), drawing_area);

    /* Set window behaviour */
    gtk_container_set_reallocate_redraws (GTK_CONTAINER(window), TRUE);
    g_signal_connect_swapped    ( G_OBJECT(window),
                                  "key_press_event",
                                  G_CALLBACK(key_handler),
                                  drawing_area );
    g_signal_connect            ( G_OBJECT(window),
                                  "destroy",
                                  G_CALLBACK(gtk_main_quit),
                                  NULL );

    return window;
}

int
main(int argc, char* argv[])
{
    /* Initialize GTK+ and GtkGLExt system */
    gtk_init(&argc, &argv);
    gtk_gl_init(&argc, &argv);

    /* Create the window and show it */
    GtkWidget *mainWindow;
    mainWindow = create_main_window();
    if (mainWindow == NULL) {
        g_print("*** Cannot create main window.\n");
        exit(1);
    }
    gtk_widget_show_all(mainWindow);

    /* Start program main loop */
    gtk_main();
    return 0;
}
