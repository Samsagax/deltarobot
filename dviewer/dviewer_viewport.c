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
 * dviewer_viewport.c :
 */

#include "dviewer_viewport.h"

#define DEFAULT_SIZE 400
#define SCROLL_DELAY_TIME 300

#define TIMEOUT_INTERVAL 25
#define BOX_SIZE 7.0

static gdouble animation_rotation = 0.0;

/* Forward declarations */
static void     d_viewport_dispose          (GObject            *obj);
static void     d_viewport_finalize         (GObject            *obj);
static void     d_viewport_size_request     (GtkWidget          *widget,
                                             GtkRequisition     *requisition);
static void     d_viewport_size_allocate    (GtkWidget          *widget,
                                             GtkAllocation      *allocation);
static void     d_viewport_realize          (GtkWidget          *widget);
static gboolean d_viewport_configure        (GtkWidget          *widget,
                                             GdkEventConfigure  *event);
static gboolean d_viewport_expose           (GtkWidget          *widget,
                                             GdkEventExpose     *event);
static gboolean d_viewport_map              (GtkWidget          *widget,
                                             GdkEvent           *event);
static gboolean d_viewport_unmap            (GtkWidget          *widget,
                                             GdkEvent           *event);
static gboolean d_viewport_visibility_notify (GtkWidget         *widget,
                                             GdkEventVisibility *event);
static gboolean d_viewport_button_press     (GtkWidget          *widget,
                                             GdkEventButton     *event);
static gboolean d_viewport_button_release   (GtkWidget          *widget,
                                             GdkEventButton     *event);
static gboolean d_viewport_motion_notify    (GtkWidget          *widget,
                                             GdkEventMotion     *event);
static GdkGLConfig* d_viewport_configure_gl (gboolean           verbose);
static gboolean d_viewport_timeout_animate  (GtkWidget          *widget);

/* Define type */
G_DEFINE_TYPE(DViewport, d_viewport, GTK_TYPE_DRAWING_AREA)

static void
d_viewport_init (DViewport  *self)
{
    g_warning("d_viewport_init is a stub");

    self->geometry = NULL;
    self->robot_pos = d_pos_new();
    self->button = 0;
    self->max_fps = 60;
    self->timer = 0;
    self->glconfig = d_viewport_configure_gl(TRUE);
    self->near_clip = 1.0;
    self->far_clip = 400.0;
    self->view_angle = 45.0;
    self->view_x_angle = 0.0;
    self->view_y_angle = 0.0;
    self->view_z_angle = 0.0;
    self->zoom = 1.0;

    gtk_widget_set_gl_capability ( GTK_WIDGET(self),
                                   self->glconfig,
                                   NULL,
                                   TRUE,
                                   GDK_GL_RGBA_TYPE);
    g_timeout_add (TIMEOUT_INTERVAL,
                   (GSourceFunc) d_viewport_timeout_animate,
                   GTK_WIDGET(self));
}

static void
d_viewport_class_init (DViewportClass   *klass)
{
    GObjectClass *gObjClass = G_OBJECT_CLASS(klass);
    GtkWidgetClass *gtkWidgetClass = GTK_WIDGET_CLASS(klass);

    gObjClass->finalize = d_viewport_finalize;
    gObjClass->dispose = d_viewport_dispose;

    gtkWidgetClass->realize = d_viewport_realize;
    gtkWidgetClass->configure_event = d_viewport_configure;
    gtkWidgetClass->expose_event = d_viewport_expose;
    gtkWidgetClass->size_request = d_viewport_size_request;
    gtkWidgetClass->size_allocate = d_viewport_size_allocate;
    gtkWidgetClass->button_press_event = d_viewport_button_press;
    gtkWidgetClass->button_release_event = d_viewport_button_release;
    gtkWidgetClass->motion_notify_event = d_viewport_motion_notify;
}

GtkWidget*
d_viewport_new (DGeometry *geometry)
{
    g_return_val_if_fail (geometry != NULL, NULL);
    g_return_val_if_fail (D_IS_GEOMETRY(geometry), NULL);

    DViewport *viewport = g_object_new(D_TYPE_VIEWPORT, NULL);
    if (viewport->geometry == NULL) {
        viewport->geometry = g_object_ref(geometry);
    }

    return GTK_WIDGET(viewport);
}

static void
d_viewport_dispose(GObject *obj)
{
    DViewport *self = D_VIEWPORT(obj);
    if (self->geometry) {
        g_object_unref(self->geometry);
        self->geometry = NULL;
    }
    if (self->robot_pos) {
        g_object_unref(self->robot_pos);
        self->robot_pos = NULL;
    }

    G_OBJECT_GET_CLASS(d_viewport_parent_class)->dispose(obj);
}

static void
d_viewport_finalize(GObject *obj)
{
    G_OBJECT_GET_CLASS(d_viewport_parent_class)->finalize(obj);
}

static void
d_viewport_size_request (GtkWidget      *widget,
                         GtkRequisition *requisition)
{
    requisition->width = DEFAULT_SIZE;
    requisition->height = DEFAULT_SIZE;
}

static void
d_viewport_size_allocate (GtkWidget     *widget,
                          GtkAllocation *allocation)
{
    g_return_if_fail (D_IS_VIEWPORT(widget));
    g_return_if_fail (allocation != NULL);

    GTK_WIDGET_CLASS(d_viewport_parent_class)->size_allocate(widget, allocation);
    widget->allocation = *allocation;

    if (gtk_widget_get_realized(widget)) {
        gdk_window_move_resize (widget->window,
                                allocation->x,
                                allocation->y,
                                allocation->width,
                                allocation->height);
    }
}

static void
d_viewport_realize (GtkWidget   *widget)
{
    DViewport *vport;
    GdkWindowAttr attributes;
    gint attributes_mask;

    g_return_if_fail(widget != NULL);
    g_return_if_fail(D_IS_VIEWPORT(widget));

    gtk_widget_add_events(widget,
                          GDK_POINTER_MOTION_MASK       |
                          GDK_POINTER_MOTION_HINT_MASK  |
                          GDK_BUTTON_PRESS_MASK         |
                          GDK_BUTTON_RELEASE_MASK);
    GTK_WIDGET_CLASS(d_viewport_parent_class)->realize(widget);

    if (!gtk_widget_is_gl_capable(widget)) {
        g_error("d_viewport_realize: widget is not GL capable");
    }
    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

    /* OpenGL BEGIN */
    if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext)) {
        return;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
//    glFrontFace(GL_CCW);
//    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    gdk_gl_drawable_gl_end(gldrawable);
    /* OpenGL END */
}

static gboolean
d_viewport_configure (GtkWidget           *widget,
                      GdkEventConfigure   *event)
{
    /* Add return_val_if_fail guards */
    DViewport *self = D_VIEWPORT(widget);

    GtkAllocation allocation;
    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

    GLfloat w;
    GLfloat h;

    gtk_widget_get_allocation (widget, &allocation);
    w = allocation.width;
    h = allocation.height;
    /* Prevent division by zero */
    if (h == 0) {
        h = 1.0;
    }
    GLfloat aspect = (GLfloat)w/(GLfloat)h;

    /* OpenGL BEGIN */
    if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext)) {
        return FALSE;
    }

    glViewport (0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gdk_gl_drawable_gl_end (gldrawable);
    /* OpenGL END */

    return FALSE;
}

static gboolean
d_viewport_expose (GtkWidget        *widget,
                   GdkEventExpose   *event)
{
    g_return_val_if_fail(widget != NULL, FALSE);
    g_return_val_if_fail(D_IS_VIEWPORT(widget), FALSE);
    g_return_val_if_fail(event != NULL, FALSE);

    if (event->count > 0) {
        return FALSE;
    }

    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

    /* OpenGL BEGIN */
    if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext)) {
        return FALSE;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();

    d_viewer_draw_reference_frame(10.0, 50.0);
    glTranslatef(0.0f, 0.0f, -20.0f);

	GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat lightPos[] = {-2 * BOX_SIZE, BOX_SIZE, 4 * BOX_SIZE, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glRotatef(-animation_rotation, 1.0f, 1.0f, 0.0f);

	glBegin(GL_QUADS);

	//Top face
	glColor3f(1.0f, 1.0f, 0.0f);
	glNormal3f(0.0, 1.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);

	//Bottom face
	glColor3f(1.0f, 0.0f, 1.0f);
	glNormal3f(0.0, -1.0f, 0.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);

	//Left face
	glNormal3f(-1.0, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);

	//Right face
	glNormal3f(1.0, 0.0f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);

    //Front face
	glNormal3f(0.0, 0.0f, 1.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, BOX_SIZE / 2);

	//Back face
	glNormal3f(0.0, 0.0f, -1.0f);
	glVertex3f(-BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(-BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, BOX_SIZE / 2, -BOX_SIZE / 2);
	glVertex3f(BOX_SIZE / 2, -BOX_SIZE / 2, -BOX_SIZE / 2);

	glEnd();

    /* Swap bufers */
    if(gdk_gl_drawable_is_double_buffered(gldrawable)) {
        gdk_gl_drawable_swap_buffers(gldrawable);
    } else {
        glFlush();
    }

    gdk_gl_drawable_gl_end (gldrawable);
    /* OpenGL END */

    return FALSE;
}

static gboolean
d_viewport_map (GtkWidget   *widget,
                GdkEvent    *event)
{
    g_warning("d_viewport_map is a stub");
    return FALSE;
}

static gboolean
d_viewport_unmap (GtkWidget *widget,
                  GdkEvent  *event)
{
    g_warning("d_viewport_unmap is a stub");
    return FALSE;
}

static gboolean
d_viewport_visibility_notify (GtkWidget             *widget,
                              GdkEventVisibility    *event)
{
    g_warning("d_viewport_visibility_notify is a stub");
    return FALSE;
}

static gboolean
d_viewport_button_press (GtkWidget      *widget,
                         GdkEventButton *event)
{
    g_return_val_if_fail (D_IS_VIEWPORT(widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    g_warning("d_viewport_button_press is a stub");
    return FALSE;
}

static gboolean
d_viewport_button_release (GtkWidget        *widget,
                           GdkEventButton   *event)
{
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (D_IS_VIEWPORT(widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    g_warning("d_viewport_button_release is a stub");
    return FALSE;
}

static gboolean
d_viewport_motion_notify (GtkWidget         *widget,
                          GdkEventMotion    *event)
{
    g_return_val_if_fail (widget != NULL, FALSE);
    g_return_val_if_fail (D_IS_VIEWPORT(widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    g_warning("d_viewport_motion_notify is a stub");
    return FALSE;
}
static GdkGLConfig*
d_viewport_configure_gl (gboolean verbose)
{
    GdkGLConfig *glconfig;
    /* Print OpenGL version */
    gint minorGLVer, majorGLVer;
    gdk_gl_query_version (&majorGLVer, &minorGLVer);
    g_print("OpenGL extension version: %d.%d\n", majorGLVer, minorGLVer);


    /* Try double-buffered visual */
    glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB    |
                                          GDK_GL_MODE_DEPTH  |
                                          GDK_GL_MODE_DOUBLE);
    if (glconfig == NULL) {
        g_print ("\n*** Cannot find the double-buffered visual.\n");
        g_print ("\n*** Trying single-buffered visual.\n");

        /* Try single-buffered visual */
        glconfig = gdk_gl_config_new_by_mode (GDK_GL_MODE_RGB   |
					                          GDK_GL_MODE_DEPTH);
        if (glconfig == NULL) {
            g_print ("*** No appropriate OpenGL-capable visual found.\n");
	    }
    }
    return glconfig;
}

static gboolean
d_viewport_timeout_animate (GtkWidget    *widget)
{
    GtkAllocation allocation;
    GdkWindow *window;

    window = gtk_widget_get_window(widget);
    gtk_widget_get_allocation (widget, &allocation);

    animation_rotation += 1.0;
    if (animation_rotation > 360) {
        animation_rotation -= 360;
    }

    gdk_window_invalidate_rect (window, &allocation, FALSE);
    gdk_window_process_updates (window, FALSE);

    return TRUE;
}
