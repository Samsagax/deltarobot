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

#include "dviewer_engine.h"

static GLdouble v_rotX = 0.0;
static GLdouble v_rotY = 0.0;
static GLdouble v_rotZ = 0.0;

/*
 * Initialize drawing area
 */
static void
draw_init (GtkWidget    *widget,
           gpointer      data)
{
    GdkGLContext    *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable   *gldrawable = gtk_widget_get_gl_drawable(widget);

    /* OpenGL BEGIN */
    if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext)) {
        return;
    }
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    /* OpenGL END */
    gdk_gl_drawable_gl_end(gldrawable);
}

/*
 * Draw scene
 */
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
    // Save current transformation
    glPushMatrix();

    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate view
    glRotatef(v_rotX , 1.0, 0.0, 0.0);
    glRotatef(v_rotY , 0.0, 1.0, 0.0);
    glRotatef(v_rotZ , 0.0, 0.0, 1.0);

    GLfloat r = 30.0;
    d_viewer_draw_reference_frame   ( 10.0, 0.5 );
    glColor3f(1.0, 0.5, 0.0);
    d_viewer_draw_platform          ( r, 2.0, 5.0 );

    {
        int i;
        for (i = 0; i < 3; i++) {
            GLfloat phi = 120.0 * (GLfloat) i;
            glPushMatrix();
            glRotatef(phi, 0.0, 0.0, 1.0);
            glTranslatef(r, 0.0, 0.0);
            glColor3f(0.5, 0.5, 1.0);
            d_viewer_draw_arm(30.0, 40.0, 1.0, 1.5708, 1.5708, 2.0);
            glPopMatrix();
        }
    }

    // Finish drawing, restore transformation
    glPopMatrix();

    if (gdk_gl_drawable_is_double_buffered (gldrawable)) {
        gdk_gl_drawable_swap_buffers (gldrawable);
    } else {
        glFlush();
    }
    gdk_gl_drawable_gl_end(gldrawable);
    /* OpenGL END */

    return TRUE;
}

static gboolean
reshape (GtkWidget          *widget,
         GdkEventConfigure  *event,
         gpointer            data )
{
    GdkGLContext    *glcontext = gtk_widget_get_gl_context (widget);
    GdkGLDrawable   *gldrawable = gtk_widget_get_gl_drawable (widget);

    GLfloat h = widget->allocation.height;
    GLfloat w = widget->allocation.width;
    // Prevent division by zero
    if (h == 0) {
        h = 1.0;
    }
    GLfloat aspect = w/h;

    /* OpenGL BEGIN */
    if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext)) {
        return FALSE;
    }

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30.0, aspect, 1.0, 400.0);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, -20.0, -200.0);
    glRotatef   (-90.0, 1.0, 0.0, 0.0);
    glRotatef   (-90.0, 0.0, 0.0, 1.0);
    glRotatef   (30.0, 0.0, 1.0, 0.0);
    glRotatef   (-45.0, 0.0, 0.0, 1.0);

    gdk_gl_drawable_gl_end(gldrawable);
    /* OpenGL END */
}

static gboolean
key_handler ( GtkWidget     *widget,
              GdkEventKey   *event,
              gpointer       data )
{
    switch (event->keyval) {
        case GDK_KEY_Page_Up:
            v_rotZ += 5.0;
            break;
        case GDK_KEY_Page_Down:
            v_rotZ -= 5.0;
            break;
        case GDK_KEY_Up:
            v_rotX += 5.0;
            break;
        case GDK_KEY_Down:
            v_rotX -= 5.0;
            break;
        case GDK_KEY_Left:
            v_rotY += 5.0;
            break;
        case GDK_KEY_Right:
            v_rotY -= 5.0;
            break;
        case GDK_KEY_Escape:
            gtk_main_quit();
            break;
        default:
            return FALSE;
    }

    gdk_window_invalidate_rect(widget->window, &widget->allocation, FALSE);
    return TRUE;
}

static void
show_about  ( GtkMenuItem   *item,
              gpointer      data )
{
    gtk_show_about_dialog   ( data,
                              "title", "About DSim Viewer",
                              "program-name", "DSim Viewer",
                              "version", "0.1",
                              "copyright", "Copyright 2012. Joaquín Ignacio Aramendía GNU GPL",
                              NULL );
}

static GtkWidget*
create_main_window(void) {
    GtkWidget       *window;
    GtkWidget       *vbox;
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
    g_object_unref(glconfig);

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

    /*
     * Create Menus
     */
    GtkWidget       *menu_bar;
    GtkWidget       *menu_item_file, *menu_item_help;
    GtkWidget       *menu_file, *menu_help;

    menu_bar = gtk_menu_bar_new();

    menu_item_file  = gtk_menu_item_new_with_label("File");
    menu_file       = gtk_menu_new();

    menu_item_help  = gtk_menu_item_new_with_label("Help");
    menu_help       = gtk_menu_new();

    GtkWidget   *menu_item_quit = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), menu_item_quit);
    g_signal_connect        ( G_OBJECT(menu_item_quit),
                              "activate",
                              G_CALLBACK(gtk_main_quit),
                              NULL );
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item_file), menu_file);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item_file);

    GtkWidget   *menu_item_about = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_help), menu_item_about);
    g_signal_connect        ( G_OBJECT(menu_item_about),
                              "activate",
                              G_CALLBACK(show_about),
                              window );
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_item_help), menu_help);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), menu_item_help);

    /* Create the window */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(window), "DSim GL Viewer");
    gtk_window_set_icon_name(GTK_WINDOW(window), "applications-system");

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

    /* Set layout */
    vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start  (GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);
    gtk_box_pack_start  (GTK_BOX(vbox), drawing_area, TRUE, TRUE, 0);
    gtk_container_add   (GTK_CONTAINER(window), vbox);

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
