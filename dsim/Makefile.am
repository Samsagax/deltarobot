AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir)
noinst_LTLIBRARIES = libdsim.la
libdsim_la_SOURCES = dsim_calc.c dsim_pos.c dsim_axes.c dsim_error.c
libdsim_la_LIBADD = -lgsl -lgslcblas -lm 

noinst_PROGRAMS = ../test/dsim
___test_dsim_SOURCES = main.c
___test_dsim_LDADD = -ldsim
