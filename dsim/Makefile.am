AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) ${gobject_CFLAGS}

sources = dsim.h dsim_vector.c dsim_pos.c dsim_axes.c dsim_trajectory.c dsim_geometry.c dsim_solver.c
noinst_LTLIBRARIES = libdsim.la
libdsim_la_SOURCES = ${sources}
libdsim_la_LIBADD = ${gobject_LIBS}

noinst_PROGRAMS = ../test/dsim
___test_dsim_SOURCES = main.c libdsim.a
___test_dsim_LDADD = -ldsim
