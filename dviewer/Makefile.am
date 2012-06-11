AM_CPPFLAGS = -I$(top_builddir) \
			-I$(top_srcdir) \
			-I../dsim \
			${gl_CFLAGS} \
			${glu_CFLAGS} \
			${gtkglext_CFLAGS}

AM_CFLAGS = -std=c99

LDADD = ${gl_LIBS} ${glu_LIBS} ${gtkglext_LIBS} ../dsim/libdsim.la -lm

sources = main-gtk.c \
			dviewer_engine.c \
			dviewer_viewport.c \
			../dsim/libdsim.la

bin_PROGRAMS = ../test/dviewer-gtk
___test_dviewer_gtk_SOURCES = ${sources}
