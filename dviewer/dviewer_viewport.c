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

/* Local variables */
enum
{
    PROP_0,
    PROP_GEOMETRY,
//    PROP_EXTAXES,
//    PROP_SCENE_CENTER,
    PROP_SCENE_DISTANCE,
    PROP_POLAR_ANGLE,
    PROP_AZIMUTH_ANGLE,
    PROP_NEAR_CLIP,
    PROP_FAR_CLIP,
    PROP_EYE_ANGLE,
    N_PROPERTIES
};

enum
{
    EXTAXES_CHANGED,
    LAST_SIGNAL
};

static GParamSpec *viewport_properties[N_PROPERTIES] = { NULL, };

/* Forward declarations */
static void     d_viewport_set_property     (GObject            *obj,
                                             guint              prop_id,
                                             const GValue       *value,
                                             GParamSpec         *pspec);

static void     d_viewport_get_property     (GObject            *obj,
                                             guint              prop_id,
                                             GValue             *value,
                                             GParamSpec         *pspec);

static void     d_viewport_set_geometry     (DViewport          *self,
                                             DGeometry          *geometry);

static void     d_viewport_destroy          (GtkObject          *obj);

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

static void     d_viewport_trackball_rotation
                                            (DViewport          *self,
                                             gint               x,
                                             gint               y);

static GdkGLConfig* d_viewport_configure_gl (gboolean           verbose);

static gboolean d_viewport_timeout_animate  (GtkWidget          *widget);

/*
 * Define type
 */
G_DEFINE_TYPE(DViewport, d_viewport, GTK_TYPE_DRAWING_AREA)

/*
 * Widget behaviour
 */
static void
d_viewport_init (DViewport  *self)
{
    self->geometry = NULL;
    self->extaxes = gsl_matrix_calloc(3, 3);
    self->button = 0;
    self->glconfig = d_viewport_configure_gl(FALSE);
    self->scene_center = gsl_vector_calloc(3);
    self->scene_distance = 300.0;
    self->polar_angle = G_PI / 4.0;
    self->azimuth_angle = G_PI / 4.0;
    self->near_clip = 1.0;
    self->far_clip = 400.0;
    self->eye_angle = 25.0;

    gtk_widget_set_gl_capability ( GTK_WIDGET(self),
                                   self->glconfig,
                                   NULL,
                                   TRUE,
                                   GDK_GL_RGBA_TYPE);
}

static void
d_viewport_class_init (DViewportClass   *klass)
{
    GObjectClass *objectclass = G_OBJECT_CLASS(klass);
    GtkObjectClass *gtkobjectclass = GTK_OBJECT_CLASS(klass);
    GtkWidgetClass *widgetlass = GTK_WIDGET_CLASS(klass);

    objectclass->set_property = d_viewport_set_property;
    objectclass->get_property = d_viewport_get_property;

    gtkobjectclass->destroy = d_viewport_destroy;

    widgetlass->realize = d_viewport_realize;
    widgetlass->configure_event = d_viewport_configure;
    widgetlass->expose_event = d_viewport_expose;
    widgetlass->size_request = d_viewport_size_request;
    widgetlass->button_press_event = d_viewport_button_press;
    widgetlass->button_release_event = d_viewport_button_release;
    widgetlass->motion_notify_event = d_viewport_motion_notify;

    viewport_properties[PROP_GEOMETRY] =
        g_param_spec_object ("geometry",
                             "Geometry",
                             "The Viewport's DGeometry object to draw the manipulator",
                             D_TYPE_GEOMETRY,
                             G_PARAM_READWRITE);

    viewport_properties[PROP_SCENE_DISTANCE] =
        g_param_spec_double ("scene-distance",
                             "Scene Center",
                             "The Viewport's distance from the center of the scene",
                             0.0,
                             G_MAXDOUBLE,
                             0.0,
                             G_PARAM_READWRITE);

    viewport_properties[PROP_POLAR_ANGLE] =
        g_param_spec_double ("polar-angle",
                             "Polar angle",
                             "The Viewport's polar angle (theta)",
                             0.0,
                             G_PI,
                             G_PI / 2.0,
                             G_PARAM_READWRITE);

    viewport_properties[PROP_AZIMUTH_ANGLE] =
        g_param_spec_double ("azimuth-angle",
                             "Azimuth angle",
                             "The Viewport's azimuth angle (phi)",
                             0.0,
                             2.0 * G_PI,
                             G_PI,
                             G_PARAM_READWRITE);

    viewport_properties[PROP_NEAR_CLIP] =
        g_param_spec_double ("near-clip",
                             "Near Clip",
                             "The Viewport's near clip distance for the 3D drawing area",
                             -G_MAXDOUBLE,
                             G_MAXDOUBLE,
                             0.0,
                             G_PARAM_READWRITE);

    viewport_properties[PROP_FAR_CLIP] =
        g_param_spec_double ("far-clip",
                             "Far Clip",
                             "The Viewport's far clip distance for the 3D drawing area",
                             -G_MAXDOUBLE,
                             G_MAXDOUBLE,
                             400.0,
                             G_PARAM_READWRITE);

    viewport_properties[PROP_EYE_ANGLE] =
        g_param_spec_double ("eye-angle",
                             "Eye Angle",
                             "The Viewport's eye angle for the 3D drawing perspective",
                             0.0,
                             180.0,
                             25.0,
                             G_PARAM_READWRITE);

    g_object_class_install_properties (objectclass,
                                       N_PROPERTIES,
                                       viewport_properties);
}

static void
d_viewport_destroy (GtkObject   *obj)
{
    DViewport *self = D_VIEWPORT(obj);
    if (self->geometry) {
        g_object_unref(self->geometry);
        self->geometry = NULL;
    }
    if (self->extaxes) {
        gsl_matrix_free(self->extaxes);
        self->extaxes = NULL;
    }
    if (self->scene_center) {
        gsl_vector_free(self->scene_center);
        self->scene_center = NULL;
    }
    if (self->glconfig) {
        g_object_unref(self->glconfig);
        self->glconfig = NULL;
    }

    GTK_OBJECT_CLASS(d_viewport_parent_class)->destroy(obj);
}

static void
d_viewport_get_property (GObject    *obj,
                         guint      prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
    DViewport *viewport = D_VIEWPORT(obj);

    switch(prop_id) {
        case PROP_GEOMETRY:
            g_value_set_object(value, viewport->geometry);
            break;
//        case PROP_EXTAXES:
//            g_value_set_object(value, viewport->extaxes);
//            break;
//        case PROP_SCENE_CENTER:
//            g_value_set_object(value, viewport->scene_center);
//            break;
        case PROP_SCENE_DISTANCE:
            g_value_set_double(value, viewport->scene_distance);
            break;
        case PROP_POLAR_ANGLE:
            g_value_set_double(value, viewport->polar_angle);
            break;
        case PROP_AZIMUTH_ANGLE:
            g_value_set_double(value, viewport->azimuth_angle);
            break;
        case PROP_NEAR_CLIP:
            g_value_set_double(value, viewport->near_clip);
            break;
        case PROP_FAR_CLIP:
            g_value_set_double(value, viewport->far_clip);
            break;
        case PROP_EYE_ANGLE:
            g_value_set_double(value, viewport->eye_angle);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
            break;
    }
}

static void
d_viewport_set_property (GObject        *obj,
                         guint          prop_id,
                         const GValue   *value,
                         GParamSpec     *pspec)
{
    DViewport *self = D_VIEWPORT(obj);

    switch (prop_id) {
        DGeometry *geometry;
//        gsl_matrix *extaxes;
//        gsl_vector *scene_center;

//        case PROP_EXTAXES:
//            extaxes = D_EXTAXES(g_value_get_object(value));
//            if (!extaxes) {
//                extaxes = d_ext_axes_new();
//            }
//            d_viewport_set_ext_axes(self, extaxes);
//            break;
        case PROP_GEOMETRY:
            geometry = D_GEOMETRY(g_value_get_object(value));
            if (!geometry) {
                g_warning("Invalid GObject type for 'geometry' property in DViewport.");
            }
            d_viewport_set_geometry(self, geometry);
            break;
//        case PROP_SCENE_CENTER:
//            scene_center = D_VECTOR(g_value_get_object(value));
//            if (!scene_center) {
//                scene_center = d_pos_new_full(0.0, 0.0, 0.0);
//            }
//            d_viewport_set_scene_center(self, scene_center);
//            break;
        case PROP_SCENE_DISTANCE:
            d_viewport_set_scene_distance(self, g_value_get_double(value));
            break;
        case PROP_POLAR_ANGLE:
            d_viewport_set_polar_angle(self, g_value_get_double(value));
            break;
        case PROP_AZIMUTH_ANGLE:
            d_viewport_set_azimuth_angle(self, g_value_get_double(value));
            break;
        case PROP_NEAR_CLIP:
            d_viewport_set_near_clip(self, g_value_get_double(value));
            break;
        case PROP_FAR_CLIP:
            d_viewport_set_far_clip(self, g_value_get_double(value));
            break;
        case PROP_EYE_ANGLE:
            d_viewport_set_eye_angle(self, g_value_get_double(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
            break;
    }
}

static void
d_viewport_set_geometry (DViewport  *self,
                         DGeometry  *geometry)
{
    if (self->geometry != geometry) {
        if(self->geometry) {
            g_object_unref(self->geometry);
        }
        self->geometry = g_object_ref(geometry);

        /* Queve Redraw */
        GtkWidget *widget = GTK_WIDGET(self);
        if (gtk_widget_get_realized(widget)) {
            gdk_window_invalidate_rect(widget->window,
                                       &widget->allocation,
                                       FALSE);
        }
    }

    g_object_notify(G_OBJECT(self), "geometry");
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
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    gdk_gl_drawable_gl_end(gldrawable);
    /* OpenGL END */
}

static gboolean
d_viewport_configure (GtkWidget           *widget,
                      GdkEventConfigure   *event)
{
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
    gluPerspective(self->eye_angle, aspect, self->near_clip, self->far_clip);

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

    DViewport *self = D_VIEWPORT(widget);

    GdkGLContext *glcontext = gtk_widget_get_gl_context(widget);
    GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable(widget);

    /* OpenGL BEGIN */
    if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext)) {
        return FALSE;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(gsl_vector_get(self->scene_center, 0) + self->scene_distance * sin(self->polar_angle) * cos(self->azimuth_angle),
              gsl_vector_get(self->scene_center, 1) + self->scene_distance * sin(self->polar_angle) * sin(self->azimuth_angle),
              gsl_vector_get(self->scene_center, 2) + self->scene_distance * cos(self->polar_angle),
              gsl_vector_get(self->scene_center, 0),
              gsl_vector_get(self->scene_center, 1),
              gsl_vector_get(self->scene_center, 2),
              -cos(self->polar_angle) * cos(self->azimuth_angle),
              -cos(self->polar_angle) * sin(self->azimuth_angle),
              sin(self->polar_angle));

	GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	GLfloat lightColor[] = {0.7f, 0.7f, 0.7f, 1.0f};
	GLfloat lightPos[] = {
        self->scene_distance * sin(self->polar_angle) * cos(self->azimuth_angle + G_PI),
        self->scene_distance * sin(self->polar_angle) * sin(self->azimuth_angle + G_PI),
        self->scene_distance * cos(self->polar_angle) };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    /*
     * Draw actors
     */
    glPushMatrix();
    d_viewer_draw_reference_frame(30.0, 0.5);
    d_viewer_draw_robot_with_ext_axes(self->geometry, self->extaxes);
    glPopMatrix();

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
//    g_warning("d_viewport_button_press is a stub");

    g_return_val_if_fail (D_IS_VIEWPORT(widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    DViewport *self = D_VIEWPORT(widget);
    if (!self->button) {
        gtk_grab_add(widget);
        self->button = event->button;
        d_viewport_trackball_rotation(self, event->x, event->y);
    }

    return FALSE;
}

static gboolean
d_viewport_button_release (GtkWidget        *widget,
                           GdkEventButton   *event)
{
//    g_warning("d_viewport_button_release is a stub");

    g_return_val_if_fail (D_IS_VIEWPORT(widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    DViewport *self = D_VIEWPORT(widget);
    if(self->button == event->button) {
        gtk_grab_remove(widget);
        self->button = 0;

        d_viewport_queve_redraw(self);
//        GtkAllocation allocation;
//        gtk_widget_get_allocation (widget, &allocation);
//        gdk_window_invalidate_rect (gtk_widget_get_window(widget), &allocation, FALSE);
    }

    return FALSE;
}

static gboolean
d_viewport_motion_notify (GtkWidget         *widget,
                          GdkEventMotion    *event)
{
    g_return_val_if_fail (D_IS_VIEWPORT(widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    DViewport *self = D_VIEWPORT(widget);

    gint x, y;
    GdkModifierType state;

    if (event->is_hint) {
        gdk_window_get_pointer(event->window, &x, &y, &state);
    } else {
        x = event->x;
        y = event->y;
        state = event->state;
    }
    if (state & GDK_BUTTON1_MASK) {
        d_viewport_trackball_rotation(self, x, y);
    }
    return FALSE;
}

static void
d_viewport_trackball_rotation (DViewport    *self,
                               gint         x,
                               gint         y)
{
//    g_warning("d_viewport_trackball_rotation is a stub");

//    g_print("Input  coordinates: %i, %i\n", x, y);
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

/*----------------------------------------------------------------------------
 *  Public API
 *--------------------------------------------------------------------------*/

GtkWidget*
d_viewport_new (DGeometry *geometry)
{
    g_return_val_if_fail (D_IS_GEOMETRY(geometry), NULL);

    gsl_vector *pos = gsl_vector_calloc(3);
    gsl_vector_set(pos, 3, (geometry->a + geometry->b) / 2.0);
    return d_viewport_new_with_pos (geometry, pos);
}

GtkWidget*
d_viewport_new_with_pos (DGeometry  *geometry,
                         gsl_vector *pos)
{
    g_return_val_if_fail(D_IS_GEOMETRY(geometry), NULL);
    g_return_val_if_fail(pos != NULL, NULL);

    DViewport *viewport;

    viewport = g_object_new(D_TYPE_VIEWPORT, NULL);

    d_viewport_set_geometry(viewport, geometry);
    d_viewport_set_pos(viewport, pos);

    return GTK_WIDGET(viewport);
}

GtkWidget*
d_viewport_new_full (DGeometry  *geometry,
                     gsl_matrix *extaxes,
                     gsl_vector *scene_center,
                     gdouble    scene_distance,
                     gdouble    polar_angle,
                     gdouble    azimuth_angle,
                     gdouble    near_clip,
                     gdouble    far_clip,
                     gdouble    eye_angle)
{
    g_return_val_if_fail (D_IS_GEOMETRY(geometry), NULL);

    DViewport *viewport;

    viewport = g_object_new(D_TYPE_VIEWPORT, NULL);

    d_viewport_set_geometry(viewport, geometry);
    d_viewport_set_ext_axes(viewport, extaxes);
    d_viewport_configure_view(viewport,
                              scene_center,
                              scene_distance,
                              polar_angle,
                              azimuth_angle,
                              near_clip,
                              far_clip,
                              eye_angle);

    return GTK_WIDGET(viewport);
}

void
d_viewport_configure_view (DViewport    *self,
                           gsl_vector     *scene_center,
                           gdouble      scene_distance,
                           gdouble      polar_angle,
                           gdouble      azimuth_angle,
                           gdouble      near_clip,
                           gdouble      far_clip,
                           gdouble      eye_angle)
{
    g_object_freeze_notify(G_OBJECT(self));

    d_viewport_set_scene_center(self, scene_center);
    d_viewport_set_scene_distance(self, scene_distance);
    d_viewport_set_azimuth_angle(self, azimuth_angle);
    d_viewport_set_polar_angle(self, polar_angle);
    d_viewport_set_near_clip(self, near_clip);
    d_viewport_set_far_clip(self, far_clip);
    d_viewport_set_eye_angle(self, eye_angle);

    g_object_thaw_notify(G_OBJECT(self));
}

void
d_viewport_set_ext_axes (DViewport  *self,
                         gsl_matrix *extaxes)
{
    g_return_if_fail(D_IS_VIEWPORT(self));
    g_return_if_fail(extaxes != NULL);

    if (self->extaxes != extaxes) {
        gsl_matrix_memcpy(self->extaxes, extaxes);

        d_viewport_queve_redraw(self);
    }

//    g_object_notify(G_OBJECT(self), "extaxes");
}

gsl_matrix*
d_viewport_get_ext_axes (const DViewport *self)
{
    g_return_val_if_fail(D_IS_VIEWPORT(self), NULL);
    return self->extaxes;
}

void
d_viewport_set_pos (DViewport   *self,
                    gsl_vector        *pos)
{
    g_return_if_fail (D_IS_VIEWPORT(self));
    g_return_if_fail (pos != NULL);

    gsl_matrix *extaxes;
    extaxes = gsl_matrix_calloc(3, 3);
    d_solver_solve_inverse(self->geometry,
                           pos,
                           NULL,
                           extaxes);

    d_viewport_set_ext_axes (self, extaxes);
    gsl_matrix_free(extaxes);
}

gsl_vector*
d_viewport_get_pos (const DViewport *self)
{
    g_return_val_if_fail (D_IS_VIEWPORT(self), NULL);

    gsl_vector *pos;
    d_solver_solve_direct_with_ext_axes(self->geometry, self->extaxes, pos);
    return pos;
}

void
d_viewport_set_scene_center (DViewport  *self,
                             gsl_vector   *scene_center)
{
    g_return_if_fail(D_IS_VIEWPORT(self));
    g_return_if_fail(scene_center != NULL);

    if (self->scene_center != scene_center) {
        if (self->scene_center) {
            g_object_unref(self->scene_center);
        }
        self->scene_center = g_object_ref(scene_center);

        d_viewport_queve_redraw(self);
    }

    g_object_notify(G_OBJECT(self), "scene-center");
}

void
d_viewport_set_scene_center_xyz (DViewport  *self,
                                 gdouble    x,
                                 gdouble    y,
                                 gdouble    z)
{
    g_return_if_fail(D_IS_VIEWPORT(self));

    gsl_vector *center;

    center = gsl_vector_alloc(3);
    gsl_vector_set(center, 0, x);
    gsl_vector_set(center, 1, y);
    gsl_vector_set(center, 2, z);
    d_viewport_set_scene_center (self, center);

    gsl_vector_free(center);
}

gsl_vector*
d_viewport_get_scene_center (const DViewport *self)
{
    g_return_val_if_fail(D_IS_VIEWPORT(self), NULL);
    return self->scene_center;
}

void
d_viewport_set_scene_distance (DViewport    *self,
                               gdouble      scene_distance)
{
    g_return_if_fail(D_IS_VIEWPORT(self));

    self->scene_distance = CLAMP(scene_distance, 0.0, G_MAXDOUBLE);
    d_viewport_queve_redraw(self);

    g_object_notify(G_OBJECT(self), "scene-distance");
}

gdouble
d_viewport_get_scene_distance (const DViewport *self)
{
    g_return_val_if_fail(D_IS_VIEWPORT(self), 0.0);
    return self->scene_distance;
}

void
d_viewport_set_polar_angle (DViewport   *self,
                            gdouble     polar_angle)
{
    g_return_if_fail(D_IS_VIEWPORT(self));

    self->polar_angle = CLAMP(polar_angle, 0.0, G_PI);

    d_viewport_queve_redraw(self);
    g_object_notify(G_OBJECT(self), "polar-angle");
}

gdouble
d_viewport_get_polar_angle (const DViewport *self)
{
    g_return_val_if_fail(D_IS_VIEWPORT(self), 0.0);
    return self->polar_angle;
}

void
d_viewport_set_azimuth_angle (DViewport *self,
                              gdouble   azimuth_angle)
{
    g_return_if_fail(D_IS_VIEWPORT(self));

    if (azimuth_angle >= 2.0 * G_PI) {
        self->azimuth_angle = azimuth_angle - 2.0 * G_PI;
    } else if (azimuth_angle < 0.0) {
        self->azimuth_angle = azimuth_angle + 2.0 * G_PI;
    } else {
        self->azimuth_angle = azimuth_angle;
    }

    d_viewport_queve_redraw(self);
    g_object_notify(G_OBJECT(self), "azimuth-angle");
}

gdouble
d_viewport_get_azimuth_angle (const DViewport *self)
{
    g_return_val_if_fail(D_IS_VIEWPORT(self), 0.0);
    return self->azimuth_angle;
}

void
d_viewport_set_far_clip (DViewport  *self,
                         gdouble    far_clip)
{
    g_return_if_fail(D_IS_VIEWPORT(self));

    self->far_clip = far_clip;

    d_viewport_queve_redraw(self);
    g_object_notify(G_OBJECT(self), "far-clip");
}

gdouble
d_viewport_get_far_clip (const DViewport *self)
{
    g_return_val_if_fail(D_IS_VIEWPORT(self), 0.0);
    return self->far_clip;
}

void
d_viewport_set_near_clip (DViewport *self,
                          gdouble   near_clip)
{
    g_return_if_fail(D_IS_VIEWPORT(self));

    self->near_clip = near_clip;

    d_viewport_queve_redraw(self);
    g_object_notify(G_OBJECT(self), "near-clip");
}

gdouble
d_viewport_get_near_clip (const DViewport *self)
{
    g_return_val_if_fail(D_IS_VIEWPORT(self), 0.0);
    return self->near_clip;
}

void
d_viewport_set_eye_angle (DViewport *self,
                          gdouble   eye_angle)
{
    g_return_if_fail(D_IS_VIEWPORT(self));

    self->eye_angle = CLAMP(eye_angle, 0.0, G_PI);

    d_viewport_queve_redraw(self);
    g_object_notify(G_OBJECT(self), "eye-angle");
}

gdouble
d_viewport_get_eye_angle (const DViewport *self)
{
    g_return_val_if_fail(D_IS_VIEWPORT(self), 0.0);
    return self->eye_angle;
}

void
d_viewport_queve_redraw (DViewport  *self)
{
    g_return_if_fail(D_IS_VIEWPORT(self));

    GtkWidget *widget = GTK_WIDGET(self);
    if (gtk_widget_get_realized(widget)) {
        GtkAllocation allocation;
        gtk_widget_get_allocation (widget, &allocation);
        gdk_window_invalidate_rect(widget->window, &allocation, FALSE);
    }
}
