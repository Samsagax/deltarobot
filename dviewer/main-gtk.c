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
#include <gdk/gdkkeysyms.h>
//#include "dviewer_engine.h"
#include "dviewer_viewport.h"

/* Global variables */
static GtkWidget    *main_window;
static DGeometry    *robot;
static DPos         *pos;

/* Forward declarations */
static GtkWidget*   create_menu_bar (GtkWidget *window);
static void         create_main_window (void);
static gboolean     key_handler (GtkWidget *widget, GdkEventKey *event, gpointer data);
static void         increment_pos (DPos *pos, gdouble dx, gdouble dy, gdouble dz);


static gboolean
key_handler (GtkWidget      *widget,
             GdkEventKey    *event,
             gpointer       data)
{
    DViewport *viewport;

    viewport = D_VIEWPORT(widget);

    switch (event->keyval) {
        case GDK_KEY_Page_Up:
            increment_pos(pos, 0.0, 0.0, 1.0);
            d_viewport_set_pos(viewport, pos);
            break;
        case GDK_KEY_Page_Down:
            increment_pos(pos, 0.0, 0.0, -1.0);
            d_viewport_set_pos(viewport, pos);
            break;
        case GDK_KEY_Up:
            increment_pos(pos, 0.0, 1.0, 0.0);
            d_viewport_set_pos(viewport, pos);
            break;
        case GDK_KEY_Down:
            increment_pos(pos, 0.0, -1.0, 0.0);
            d_viewport_set_pos(viewport, pos);
            break;
        case GDK_KEY_Left:
            increment_pos(pos, 1.0, 0.0, 0.0);
            d_viewport_set_pos(viewport, pos);
            break;
        case GDK_KEY_Right:
            increment_pos(pos, -1.0, 0.0, 0.0);
            d_viewport_set_pos(viewport, pos);
            break;
        case GDK_KEY_Escape:
            gtk_main_quit();
            break;
        default:
            return FALSE;
    }

    return TRUE;
}

static void
increment_pos (DPos     *pos,
               gdouble  dx,
               gdouble  dy,
               gdouble  dz)
{
    DVector3 *dp;

    dp = d_pos_new_full (dx, dy, dz);

    d_vector3_add(D_VECTOR3(pos), dp);

    g_object_unref(dp);
}

static void
show_about (void)
{
    gtk_show_about_dialog   ( GTK_WINDOW(main_window),
                              "title", "About DSim Viewer",
                              "program-name", "DSim Viewer",
                              "version", "0.1",
                              "copyright", "Copyright 2012. Joaquín Ignacio Aramendía GNU GPL",
                              NULL );
}

/*
 * Create main window
 */
static void
create_main_window (void)
{
    GtkWidget       *main_vbox;
    GtkWidget       *viewport;
    GtkWidget       *menu_bar;

    /*
     * Create the window
     */
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(main_window), "DSim GL Viewer");
    gtk_window_set_icon_name(GTK_WINDOW(main_window), "applications-system");

    /*
     * Set up drawing area
    */
    viewport = d_viewport_new_with_pos(robot, pos);

    /*
     * Create Menus
     */
    menu_bar = create_menu_bar(main_window);

    /*
     * Set window behaviour
     */
    gtk_container_set_reallocate_redraws (GTK_CONTAINER(main_window), TRUE);
    g_signal_connect_swapped (G_OBJECT(main_window),
                              "key_press_event",
                              G_CALLBACK(key_handler),
                              viewport );
    g_signal_connect (G_OBJECT(main_window),
                      "destroy",
                      G_CALLBACK(gtk_main_quit),
                      NULL);


    /* Set layout */
    main_vbox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), menu_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), viewport, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(main_window), main_vbox);
}

/*
 * Create menu bar
 */
static GtkWidget*
create_menu_bar (GtkWidget  *window)
{
    static GtkItemFactoryEntry menu_items[] = {
        { "/_File",         NULL,           NULL,           0,  "<Branch>" },
        { "/File/_Quit",    "<control>Q",   gtk_main_quit,  0,  "<StockItem>",  GTK_STOCK_QUIT },
        { "/_Help",         NULL,           NULL,           0,  "<Branch>" },
        { "/Help/_About",  NULL,           show_about,     0,  "<StockItem>",  GTK_STOCK_ABOUT },
    };

    static gint nmenu_items = sizeof(menu_items) / sizeof(menu_items[0]);

    GtkItemFactory *item_factory;
    GtkAccelGroup *accel_group;

    accel_group = gtk_accel_group_new();

    item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<Main>", accel_group);

    gtk_item_factory_create_items (item_factory, nmenu_items, menu_items, NULL);
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    return gtk_item_factory_get_widget(item_factory, "<Main>");
}

/*
 * Main function
 */
int
main(int argc, char* argv[])
{
    /*
     * Initialize GTK+ and GtkGLExt system
     */
    gtk_init(&argc, &argv);
    gtk_gl_init(&argc, &argv);

    robot = d_geometry_new(30.0, 50.0, 30.0, 10.0);
    pos   = d_pos_new_full(0.0, 0.0, 50.0);

    /*
     * Create the window and show it
     */
    create_main_window();
    gtk_widget_show_all(main_window);

    /* Start program main loop */
    gtk_main();

    g_object_unref(robot);
    g_object_unref(pos);

    return 0;
}
