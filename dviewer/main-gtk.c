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

#include <gdk/gdkkeysyms.h>
#include "dviewer_viewport.h"

/* Global variables */
static GtkWidget            *main_window;
static DGeometry            *robot;
static DTrajectoryControl   *trajcontrol;
static DPos                 *pos;

/* Joint motion */
static GtkWidget            *go_button_joint;
static GtkWidget            *fine_check;
static GtkWidget            *axis_controls[3];

/* Forward declarations */
static GtkWidget*   create_menu_bar (GtkWidget *window);
static GtkWidget*   create_controls (void);
static void         create_main_window (void);
static gboolean     key_handler (GtkWidget *widget, GdkEventKey *event, gpointer data);
static void         increment_pos (DPos *pos, gdouble dx, gdouble dy, gdouble dz);
static void         go_button_joint_clicked (GtkButton *button, gpointer data);

/*
 * Handle key pressed on main window
 */
static gboolean
key_handler (GtkWidget      *widget,
             GdkEventKey    *event,
             gpointer       data)
{
    DViewport *viewport;

    viewport = D_VIEWPORT(widget);

    switch (event->keyval) {
        case GDK_KEY_Return:
            gtk_button_clicked(go_button_joint);
            break;
        case GDK_KEY_t:
            break;
        case GDK_KEY_g:
            break;
        case GDK_KEY_y:
            break;
        case GDK_KEY_h:
            break;
        case GDK_KEY_r:
            break;
        case GDK_KEY_f:
            break;
        case GDK_KEY_Page_Up:
            d_viewport_set_scene_distance(viewport,
                    d_viewport_get_scene_distance(viewport) - 1.0);
            break;
        case GDK_KEY_Page_Down:
            d_viewport_set_scene_distance(viewport,
                    d_viewport_get_scene_distance(viewport) + 1.0);
            break;
        case GDK_KEY_Up:
            d_viewport_set_polar_angle(viewport,
                    d_viewport_get_polar_angle(viewport) - G_PI / 180.0);
            break;
        case GDK_KEY_Down:
            d_viewport_set_polar_angle(viewport,
                    d_viewport_get_polar_angle(viewport) + G_PI / 180.0);
            break;
        case GDK_KEY_Left:
            d_viewport_set_azimuth_angle(viewport,
                    d_viewport_get_azimuth_angle(viewport) + G_PI / 180.0);
            break;
        case GDK_KEY_Right:
            d_viewport_set_azimuth_angle(viewport,
                    d_viewport_get_azimuth_angle(viewport) - G_PI / 180.0);
            break;
        default:
            return FALSE;
    }

    return TRUE;
}

static void
go_button_joint_clicked (GtkButton  *button,
                         gpointer   data)
{
    DTrajectoryCommand *cmd;
    DVector *axes;

    axes = d_axes_new();
    for (int i = 0; i < d_vector_length(axes); i++) {
        d_vector_set (axes, i, gtk_spin_button_get_value(axis_controls[i]));
    }
    cmd = d_trajectory_command_new(OT_MOVEJ, axes);
    d_trajectory_control_push_order(trajcontrol, cmd);
    if (gtk_toggle_button_get_active(fine_check)) {
        d_trajectory_control_push_order(trajcontrol, cmd);
    }

    g_object_unref(axes);
    g_object_unref(cmd);
}

static void
increment_pos (DPos     *pos,
               gdouble  dx,
               gdouble  dy,
               gdouble  dz)
{
    DVector *dp;

    dp = d_pos_new_full (dx, dy, dz);

    d_vector_add(D_VECTOR(pos), dp);

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

static void
d_trajectory_viewport_link (DVector     *axes,
                            gpointer    *data)
{
    g_return_if_fail(D_IS_VIEWPORT(data));
    DViewport *viewport = D_VIEWPORT(data);

    g_message("Current Axes: %f, %f, %f",
                d_vector_get(axes, 0),
                d_vector_get(axes, 1),
                d_vector_get(axes, 2));
    DVector *pos = d_pos_new();
    d_solver_solve_direct(robot, axes, pos);

    d_viewport_set_pos(viewport, pos);
    g_object_unref(pos);
}

/*
 * Create main window
 */
static void
create_main_window (void)
{
    GtkWidget *main_vbox;
    GtkWidget *viewport;
    GtkWidget *controls;
    GtkWidget *menu_bar;

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
    d_viewport_set_scene_center_xyz(D_VIEWPORT(viewport), 0.0, 0.0, 30.0);
    d_trajectory_control_set_output_func(trajcontrol, d_trajectory_viewport_link, viewport);

    /*
     * Create the controls
     */
    controls = create_controls();

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
    g_signal_connect_swapped (G_OBJECT(main_window),
                              "destroy",
                              G_CALLBACK(d_trajectory_control_stop),
                              trajcontrol);
    g_signal_connect_swapped (G_OBJECT(main_window),
                              "destroy",
                              G_CALLBACK(gtk_main_quit),
                              NULL);


    /* Set layout */
    main_vbox = gtk_vbox_new(FALSE, 0);

    gtk_box_pack_start(GTK_BOX(main_vbox), menu_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), viewport, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), controls, FALSE, FALSE, 0);
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
        { "/File/_Quit",    "<control>X",   gtk_main_quit,  0,  "<StockItem>",  GTK_STOCK_QUIT },
        { "/_Help",         NULL,           NULL,           0,  "<Branch>" },
        { "/Help/_About",   NULL,           show_about,     0,  "<StockItem>",  GTK_STOCK_ABOUT },
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

static GtkWidget*
create_controls (void)
{
    GtkWidget *hbox;
    GtkWidget *table;

    /*
     * Table with spinbuttons
     */
    guint rows = 1;
    guint columns = 6;
    table = gtk_table_new(rows, columns, TRUE);

    GtkWidget *axis_labels[] = {
        gtk_label_new("Axis 1:"),
        gtk_label_new("Axis 2:"),
        gtk_label_new("Axis 3:"),
    };

    axis_controls[0] = gtk_spin_button_new_with_range(-G_PI/8.0, G_PI/2.0, G_PI/180.0);
    axis_controls[1] = gtk_spin_button_new_with_range(-G_PI/8.0, G_PI/2.0, G_PI/180.0);
    axis_controls[2] = gtk_spin_button_new_with_range(-G_PI/8.0, G_PI/2.0, G_PI/180.0);

    for (int i = 0; i < 3; i++) {
        gtk_table_attach_defaults(GTK_TABLE(table),
                                  axis_labels[i],
                                  2*i, 2*i+1,
                                  0, 1);
        gtk_table_attach_defaults(GTK_TABLE(table),
                                  axis_controls[i],
                                  2*i+1, 2*i+2,
                                  0, 1);
    }

    /*
     * Fine trajectory check
     */
    fine_check = gtk_toggle_button_new_with_label("Fine");

    /*
     * Go! Button
     */
    go_button_joint = gtk_button_new_with_label("GO!");
    g_signal_connect(G_OBJECT(go_button_joint),
                     "clicked",
                     G_CALLBACK(go_button_joint_clicked),
                     NULL);

    /*
     * Horizontal Box
     */
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), table, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), fine_check, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), go_button_joint, FALSE, FALSE, 0);

    return hbox;
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

    robot = d_geometry_new(30.0, 50.0, 25.0, 10.0);
    pos   = D_POS(d_pos_new_full(0.0, 0.0, 50.0));

    /*
     * Trajectory control
     */
    trajcontrol = d_trajectory_control_new();

    /*
     * Create the window and show it
     */
    create_main_window();
    gtk_widget_show_all(main_window);

    d_trajectory_control_start(trajcontrol);

    /* Start program main loop */
    gtk_main();

    d_trajectory_control_stop(trajcontrol);
    g_object_unref(trajcontrol);
    g_object_unref(robot);
    g_object_unref(pos);

    return 0;
}
