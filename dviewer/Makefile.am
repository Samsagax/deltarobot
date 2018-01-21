AM_CPPFLAGS = -I$(top_builddir) \
			-I$(top_srcdir) \
			${gl_CFLAGS} \
			${glu_CFLAGS} \
			${gtkglext_CFLAGS}

AM_CFLAGS = -std=c99

LDADD = ${gl_LIBS} \
		${glu_LIBS} \
		${gtkglext_LIBS} \
		${gsl_LIBS} \
		../lib/libdsim.la

dv_sources = main-dv-gtk.c

dtv_sources = main-dtv-gtk.c

common_sources = dviewer_engine.c \
			dviewer_viewport.c \
			../lib/libdsim.la

bin_PROGRAMS = ../test/dviewer-gtk ../test/dtrajviewer-gtk

___test_dviewer_gtk_SOURCES = ${dv_sources} ${common_sources}

___test_dtrajviewer_gtk_SOURCES = ${dtv_sources} ${common_sources}
