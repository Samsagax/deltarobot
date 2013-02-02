/*
 * Copyright (c) 2012,
 * Author: Joaquín Ignacio Aramendía (samsagax) <samsagax [at] gmail [dot] com>
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

#include <gtk/gtk.h>
#include <dsim/dsim.h>
#include <gtkdatabox.h>
#include <gtkdatabox_lines.h>

/* Forward declarations */
static DDynamicModel *dynamic_model;
static GtkWidget *interval_spin[2];
static GtkWidget *databox;
static GtkDataboxGraph *graph[3] = { NULL };

static GtkWidget*   create_main_window          (void);
static GtkWidget*   create_menu_bar             (GtkWidget  *window);
static GtkWidget*   create_controls             (void);
static void         setup_robot                 (void);
static void         calculate_button_callback   (GtkButton  *button,
                                                 GdkEvent   *event,
                                                 gpointer   user_data);

static gfloat *theta_data[3] = { NULL, NULL, NULL };
static gfloat *time_data = NULL;
static gfloat *theta_dot_data[3] = { NULL, NULL, NULL };

static GdkColor red = {0, 65535, 0, 0};
static GdkColor green = {0, 0, 65535, 0};
static GdkColor blue = {0, 0, 0, 65535};

/*
 * Create menu bar
 */
static GtkWidget*
create_menu_bar (GtkWidget  *window)
{
    GtkWidget *menu_bar;
    GtkItemFactory *item_factory;
    GtkAccelGroup *accel_group;
    GtkItemFactoryEntry menu_items[] = {
        { "/_File",         NULL,           NULL,           0,  "<Branch>" },
        { "/File/_Quit",    "<control>Q",   gtk_main_quit,  0,  "<StockItem>",  GTK_STOCK_QUIT },
    };
    gint nmenu_items = sizeof (menu_items) / sizeof(menu_items[0]);

    accel_group = gtk_accel_group_new();
    item_factory = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<Main>", accel_group);
    gtk_item_factory_create_items(item_factory, nmenu_items, menu_items, NULL);
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    menu_bar = gtk_item_factory_get_widget(item_factory, "<Main>");
    return menu_bar;
}

/*
 * Calculate button callback
 */
static void
calculate_button_callback (GtkButton    *button,
                           GdkEvent     *event,
                           gpointer     user_data)
{
    gsl_vector *axes = NULL;
    gsl_vector *speed = NULL;
    gdouble time = 0.0;
    gdouble interval = 0.0;
    gdouble step = 0.0;
    gint len;

    interval = gtk_spin_button_get_value(GTK_SPIN_BUTTON(interval_spin[0]));
    step = gtk_spin_button_get_value(GTK_SPIN_BUTTON(interval_spin[1]));

    if (step > interval) {
        g_warning("Time step is larger than interval");
        return;
    }

    g_print("Interval: %f\n", interval);
    g_print("Time step: %f\n", step);
    g_print("Processing...\n");

    /* Clear all graphs */
    gtk_databox_graph_remove_all(GTK_DATABOX(databox));

//    len = ceil(interval / step) + 1;
//    for (int i = 0; i < 3; i++) {
//        if (theta_data[i]) {
//            g_free(theta_data[i]);
//        }
//        theta_data[i] = g_new0(gfloat, len);
//        if (theta_dot_data[i]){
//            g_free(theta_dot_data[i]);
//        }
//        theta_dot_data[i] = g_new0(gfloat, len);
//    }
//    if (time_data) {
//        g_free(time_data);
//    }
//    time_data = g_new0(gfloat, len);
//    gint j = 0;

    while (time <= interval) {
        axes = d_dynamic_model_get_axes(dynamic_model);
        speed = d_dynamic_model_get_speed(dynamic_model);
//        for (int i = 0; i < 3; i++) {
//            theta_data[i][j] = d_vector_get(axes, i);
//            theta_dot_data[i][j] = d_vector_get(speed, i);
//        }
//        time_data[j] = time;

        g_print("t = %f, [%f, %f, %f]\n",
                time,
                gsl_vector_get(axes, 0),
                gsl_vector_get(axes, 1),
                gsl_vector_get(axes, 2));

        d_dynamic_model_solve_inverse(dynamic_model, step);
        time += step;
//        j++;
    }

    g_print("Plotting data...\n");

    /* Add them one clolor each */
//    GdkColor colors[3] = {
//        red,
//        green,
//        blue
//    };
//    for (int i = 0; i < 3; i++) {
//        if (graph[i]) {
//            g_object_unref(graph[i]);
//        }
//        graph[i] = gtk_databox_lines_new(len,
//                    time_data,
//                    theta_data[i],
//                    &colors[i],
//                    1.0);
//        gtk_databox_graph_add(GTK_DATABOX(databox), graph[i]);
//    }
//    gtk_databox_auto_rescale(GTK_DATABOX(databox), 0.1);
    g_print("Done\n");

    return;
}

static GtkWidget*
create_controls (void)
{
    GtkWidget *controls_table;

    /*
     * Create container table
     */
    controls_table = gtk_table_new(3, 2, FALSE);

    /*
     * Spinners with labels
     */
    GtkWidget *label[2];

    label[0] = gtk_label_new("Intervalo (s):");
    interval_spin[0] = gtk_spin_button_new_with_range(0.1, 1000.0, 0.001);
    gtk_table_attach_defaults(GTK_TABLE(controls_table),
                              label[0],
                              0, 1,
                              0, 1);
    gtk_table_attach_defaults(GTK_TABLE(controls_table),
                              interval_spin[0],
                              1, 2,
                              0, 1);
    label[1] = gtk_label_new("Muestreo (s):");
    interval_spin[1] = gtk_spin_button_new_with_range(0.001, 10.0, 0.001);
    gtk_table_attach_defaults(GTK_TABLE(controls_table),
                              label[1],
                              0, 1,
                              1, 2);
    gtk_table_attach_defaults(GTK_TABLE(controls_table),
                              interval_spin[1],
                              1, 2,
                              1, 2);


    GtkWidget *calculate_button;
    calculate_button = gtk_button_new_with_label("Calcular");
    gtk_table_attach_defaults(GTK_TABLE(controls_table),
                              calculate_button,
                              0, 2,
                              2, 3);
    g_signal_connect(G_OBJECT(calculate_button),
                              "button-press-event",
                              G_CALLBACK(calculate_button_callback),
                              NULL);

    return controls_table;
}


/*
 * Create the main application window
 */
static GtkWidget*
create_main_window (void)
{
    GtkWidget *main_window;
    GtkWidget *menu_bar;
    GtkWidget *main_vbox;
    GtkWidget *box_table;
    GtkWidget *controls_table;

    /*
     * Top-level window
     */
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "DSim dynamic simulator");
    gtk_window_set_icon_name(GTK_WINDOW(main_window), "applications-system");
    g_signal_connect_swapped(G_OBJECT(main_window),
                            "destroy",
                            G_CALLBACK(gtk_main_quit),
                            NULL);

    /*
     * Main vertical box
     */
    main_vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(main_window), main_vbox);

    /*
     * Create a menu bar and pack it
     */
    menu_bar = create_menu_bar(main_window);
    gtk_box_pack_start(GTK_BOX(main_vbox), menu_bar, FALSE, FALSE, 0);

    /*
     * Create graphs
     */
    gtk_databox_create_box_with_scrollbars_and_rulers(&databox,
                                                        &box_table,
                                                        TRUE,
                                                        TRUE,
                                                        TRUE,
                                                        TRUE);
    gtk_widget_set_size_request(box_table, 600, 400);
    gtk_box_pack_start(GTK_BOX(main_vbox), box_table, TRUE, TRUE, 0);

    /*
     * Create controls
     */
    controls_table = create_controls();
    gtk_box_pack_start(GTK_BOX(main_vbox), controls_table, FALSE, FALSE, 0);



    return main_window;
}

static void
setup_robot (void)
{
    DManipulator *manipulator;
    DGeometry *geometry;
    DDynamicSpec *dynamic_spec;
    gsl_vector *gravity;
    gsl_vector *home;
    gsl_vector *stop;
    gsl_vector *torque;

    g_print("Setting up robot simulator... ");
    dynamic_spec = d_dynamic_spec_new();
    geometry = d_geometry_new(30.0, 50.0, 25.0, 10.0);
    manipulator = d_manipulator_new(geometry, dynamic_spec);

    gravity = gsl_vector_calloc(3);
    gsl_vector_set(gravity, 0, 0.0);
    gsl_vector_set(gravity, 1, 0.0);
    gsl_vector_set(gravity, 2, 9.806);

    torque = gsl_vector_calloc(3);
    gsl_vector_set(torque, 0, 0.0);
    gsl_vector_set(torque, 1, 0.0);
    gsl_vector_set(torque, 2, 0.0);

    home = gsl_vector_calloc(3);
    gsl_vector_set(home, 0, 1.0);
    gsl_vector_set(home, 1, 1.0);
    gsl_vector_set(home, 2, 1.0);

    stop= gsl_vector_calloc(3);

    dynamic_model = d_dynamic_model_new(manipulator);
    d_dynamic_model_set_axes(dynamic_model, home);
    d_dynamic_model_set_gravity(dynamic_model, gravity);
    d_dynamic_model_set_speed(dynamic_model, stop);
    d_dynamic_model_set_torque(dynamic_model, torque);

    g_object_unref(geometry);
    g_object_unref(dynamic_spec);
    g_object_unref(manipulator);
    gsl_vector_free(gravity);
    gsl_vector_free(home);
    gsl_vector_free(stop);
    gsl_vector_free(torque);

    g_print("HECHO\n");
}

gint
main (gint argc, gchar* argv[])
{
    GtkWidget *main_window;

    /*
     * Initialize GTK+ library
     */
    gtk_init(&argc, &argv);

    setup_robot();

    /*
     * Create main window and show it
     */
    main_window = create_main_window();
    gtk_widget_show_all(main_window);

    /*
     * Start program main loop
     */
    gtk_main();

    g_object_unref(dynamic_model);
    for(int i = 0; i < 3; i++) {
        g_free(theta_data[i]);
        g_free(theta_dot_data[i]);
    }
    g_free(time_data);

    return 0;
}
