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
#include <dsim/dsim.h>
#include "dviewer_viewport.h"

/* Global variables */
static GtkWidget            *main_window;
static DGeometry            *robot;
static DTrajectoryControl   *trajcontrol;
static gsl_vector           *pos;

/* Joint motion */
static GtkWidget            *go_button_joint;
static GtkWidget            *go_button_linear;
static GtkWidget            *fine_check;
static GtkWidget            *axis_controls[3];
static GtkWidget            *pos_controls[3];

/* Forward declarations */
static GtkWidget*   create_menu_bar (GtkWidget *window);
static GtkWidget*   create_controls (void);
static void         create_main_window (void);
static gboolean     key_handler (GtkWidget *widget, GdkEventKey *event, gpointer data);
static void         increment_pos (gsl_vector *pos, gdouble dx, gdouble dy, gdouble dz);
static void         go_button_joint_clicked (GtkButton *button, gpointer data);
static void         go_button_linear_clicked (GtkButton *button, gpointer data);
static void         sync_spinners_axes(gsl_vector *pos);
static void         sync_spinners_pos(gsl_vector *axes);

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
//            gtk_button_clicked(go_button_joint);
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
sync_spinners_axes(gsl_vector *pos)
{
    gsl_vector *axes = gsl_vector_calloc(3);
    d_solver_solve_inverse(robot, pos, axes, NULL, NULL);
    for (int i=0; i < pos->size; i++) {
        gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(axis_controls[i]),
            gsl_vector_get(axes, i));
    }
    gsl_vector_free(axes);
}

static void
sync_spinners_pos(gsl_vector *axes)
{
    gsl_vector *pos = gsl_vector_calloc(3);
    d_solver_solve_direct(robot, axes, pos, NULL);
    for (int i=0; i < axes->size; i++) {
        gtk_spin_button_set_value(
            GTK_SPIN_BUTTON(pos_controls[i]),
            gsl_vector_get(pos, i));
    }
    gsl_vector_free(pos);
}

static void
go_button_joint_clicked (GtkButton  *button,
                         gpointer   data)
{
    DTrajectoryCommand *cmd;
    gsl_vector *axes;

    axes = gsl_vector_calloc(3);
    for (int i = 0; i < axes->size; i++) {
        gsl_vector_set (axes, i,
            gtk_spin_button_get_value(GTK_SPIN_BUTTON(axis_controls[i])));
    }
    cmd = d_trajectory_command_new(OT_MOVEJ, axes);
    d_trajectory_control_push_order(trajcontrol, cmd);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fine_check))) {
        d_trajectory_control_push_order(trajcontrol, cmd);
    }
    sync_spinners_pos(axes);

    g_object_unref(cmd);
}

static void
go_button_linear_clicked (GtkButton *button,
                          gpointer  data)
{
    DTrajectoryCommand *cmd;
    gsl_vector *pos;

    pos = gsl_vector_calloc(3);
    for (int i = 0; i < pos->size; i++) {
        gsl_vector_set (pos, i,
            gtk_spin_button_get_value(GTK_SPIN_BUTTON(pos_controls[i])));
    }
    cmd = d_trajectory_command_new(OT_MOVEL, pos);
    d_trajectory_control_push_order(trajcontrol, cmd);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(fine_check))) {
        d_trajectory_control_push_order(trajcontrol, cmd);
    }
    sync_spinners_axes(pos);

    g_object_unref(cmd);
}

static void
increment_pos (gsl_vector     *pos,
               gdouble  dx,
               gdouble  dy,
               gdouble  dz)
{
    gsl_vector *dp;

    dp = gsl_vector_alloc(3);
    gsl_vector_set(dp, 0, dx);
    gsl_vector_set(dp, 1, dy);
    gsl_vector_set(dp, 2, dz);

    gsl_vector_add(pos, dp);

    gsl_vector_free(dp);
}

static void
show_about (void)
{
    gtk_show_about_dialog   ( GTK_WINDOW(main_window),
                              "title", "About DSim TrajViewer",
                              "program-name", "DSim TrajViewer",
                              "version", "0.1",
                              "copyright", "Copyright 2012-2018. Joaquín Ignacio Aramendía GNU GPL",
                              NULL );
}

static void
d_trajectory_viewport_link_joint (gsl_vector    *axes,
                                  gpointer      data)
{
    g_return_if_fail(D_IS_VIEWPORT(data));
    DViewport *viewport = D_VIEWPORT(data);

    g_message("Current Axes: %f, %f, %f",
                gsl_vector_get(axes, 0),
                gsl_vector_get(axes, 1),
                gsl_vector_get(axes, 2));
    gsl_vector *pos = gsl_vector_calloc(3);
    d_solver_solve_direct(robot, axes, pos, NULL);

    d_viewport_set_pos(viewport, pos);
    gsl_vector_free(pos);
}

static void
d_trajectory_viewport_link_linear (gsl_vector   *pos,
                                   gpointer     data)
{
    g_return_if_fail(D_IS_VIEWPORT(data));
    DViewport *viewport = D_VIEWPORT(data);

    g_message("Current Pos: %f, %f, %f",
                gsl_vector_get(pos, 0),
                gsl_vector_get(pos, 1),
                gsl_vector_get(pos, 2));

    d_viewport_set_pos(viewport, pos);
}

/*
 * Create main window
 */
static void
create_main_window (void)
{
    GtkWidget *main_vbox;
    GtkWidget *central_hbox;
    GtkWidget *viewport;
    GtkWidget *controls;
    GtkWidget *menu_bar;

    /*
     * Create the window
     */
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW(main_window), "DSim GL Traj Viewer");
    gtk_window_set_icon_name(GTK_WINDOW(main_window), "applications-system");

    /*
     * Set up drawing area
    */
    viewport = d_viewport_new_with_pos(robot, pos);
    d_viewport_set_scene_center_xyz(D_VIEWPORT(viewport), 0.0, 0.0, 30.0);
    d_trajectory_control_set_joint_out_fun(trajcontrol,
            d_trajectory_viewport_link_joint,
            viewport);
    d_trajectory_control_set_linear_out_fun(trajcontrol,
            d_trajectory_viewport_link_linear,
            viewport);

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
    central_hbox = gtk_hbox_new(FALSE, 0);
    GtkWidget *control_align = gtk_alignment_new(0, 0, 0, 0);

    gtk_container_add(GTK_CONTAINER(control_align), controls);

    gtk_box_pack_start(GTK_BOX(central_hbox), control_align, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(central_hbox), viewport, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(main_vbox), menu_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(main_vbox), central_hbox, TRUE, TRUE, 0);
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
    GtkWidget *table;

    /*
     * Table with spinbuttons
     */
    guint rows = 7;
    guint columns = 4;
    table = gtk_table_new(rows, columns, FALSE);

    GtkWidget *axis_labels[] = {
        gtk_label_new("Axis 1:"),
        gtk_label_new("Axis 2:"),
        gtk_label_new("Axis 3:")
    };
    GtkWidget *pos_labels[] = {
        gtk_label_new("Pos X:"),
        gtk_label_new("Pos Y:"),
        gtk_label_new("Pos Z:")
    };

    axis_controls[0] = gtk_spin_button_new_with_range(-G_PI/8.0, G_PI/2.0, G_PI/180.0);
    axis_controls[1] = gtk_spin_button_new_with_range(-G_PI/8.0, G_PI/2.0, G_PI/180.0);
    axis_controls[2] = gtk_spin_button_new_with_range(-G_PI/8.0, G_PI/2.0, G_PI/180.0);

    pos_controls[0] = gtk_spin_button_new_with_range(-50.0, 50.0, 1.0);
    pos_controls[1] = gtk_spin_button_new_with_range(-50.0, 50.0, 1.0);
    pos_controls[2] = gtk_spin_button_new_with_range(20.0, 70.0, 1.0);

    for (int i = 0; i < 3; i++) {
        gtk_table_attach_defaults(GTK_TABLE(table),
                                  axis_labels[i],
                                  0, 1,
                                  i, i+1);
        gtk_table_attach_defaults(GTK_TABLE(table),
                                  axis_controls[i],
                                  1, 2,
                                  i, i+1);
        gtk_table_attach_defaults(GTK_TABLE(table),
                                  pos_labels[i],
                                  2, 3,
                                  i, i+1);
        gtk_table_attach_defaults(GTK_TABLE(table),
                                  pos_controls[i],
                                  3, 4,
                                  i, i+1);
    }

    /*
     * Fine trajectory check
     */
    fine_check = gtk_toggle_button_new_with_label("Fine");
    gtk_table_attach_defaults(GTK_TABLE(table),
                                fine_check,
                                0, 4,
                                4, 5);

    /*
     * Go! Buttons
     */
    go_button_joint = gtk_button_new_with_label("MoveJ");
    g_signal_connect(G_OBJECT(go_button_joint),
                     "clicked",
                     G_CALLBACK(go_button_joint_clicked),
                     NULL);
    go_button_linear = gtk_button_new_with_label("MoveL");
    g_signal_connect(G_OBJECT(go_button_linear),
                     "clicked",
                     G_CALLBACK(go_button_linear_clicked),
                     NULL);

    gtk_table_attach_defaults(GTK_TABLE(table),
                                go_button_joint,
                                0, 2,
                                5, 6);
    gtk_table_attach_defaults(GTK_TABLE(table),
                                go_button_linear,
                                2, 4,
                                5, 6);

    return table;
}

static void
setup_robot (void)
{
    robot = d_geometry_new(30.0, 50.0, 25.0, 10.0);
    pos   = gsl_vector_calloc(3);
    gsl_vector_set(pos, 2, 50.0);

    /*
     * Trajectory control
     */
    trajcontrol = d_trajectory_control_new();

    d_trajectory_control_start(trajcontrol);
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

    setup_robot();

    /*
     * Create the window and show it
     */
    create_main_window();
    gtk_widget_show_all(main_window);


    /* Start program main loop */
    gtk_main();

    d_trajectory_control_stop(trajcontrol);
    g_object_unref(trajcontrol);
    g_object_unref(robot);
    gsl_vector_free(pos);

    return 0;
}
