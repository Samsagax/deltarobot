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

/* Forward declarations */
static GtkWidget*   create_main_window      (void);
static GtkWidget*   create_menu_bar         (GtkWidget  *window);
static void         setup_robot             (void);

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
 * Create the main application window
 */
static GtkWidget*
create_main_window (void)
{
    GtkWidget *main_window;
    GtkWidget *menu_bar;
    GtkWidget *main_vbox;
    GtkWidget *box_table;
    GtkWidget *data_box;

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
    gtk_databox_create_box_with_scrollbars_and_rulers(&data_box,
                                                        &box_table,
                                                        TRUE,
                                                        TRUE,
                                                        TRUE,
                                                        TRUE);
    gtk_box_pack_start(GTK_BOX(main_vbox), box_table, TRUE, TRUE, 0);

    return main_window;
}

gint
main (gint argc, gchar* argv[])
{
    GtkWidget *main_window;

    /*
     * Initialize GTK+ library
     */
    gtk_init(&argc, &argv);

    //setup_robot();

    /*
     * Create main window and show it
     */
    main_window = create_main_window();
    gtk_widget_show_all(main_window);

    /*
     * Start program main loop
     */
    gtk_main();

    return 0;
}
