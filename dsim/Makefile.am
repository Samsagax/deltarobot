AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	${gobject_CFLAGS} \
	${gsl_CFLAGS}

AM_CFLAGS = -std=gnu99

sources = \
	dsim.h \
	dsim_vector.c \
	dsim_matrix.c \
	dsim_pos.c \
	dsim_axes.c \
	dsim_speed.c \
	dsim_geometry.c \
	dsim_solver.c \
	dsim_trajectory.c \
	dsim_trajectory_joint.c \
	dsim_trajectory_linear.c \
	dsim_trajectory_command.c \
	dsim_trajectory_control.c 

lib_LTLIBRARIES = ../lib/libdsim.la
___lib_libdsim_la_SOURCES = ${sources}
___lib_libdsim_la_LIBADD = ${gobject_LIBS} ${gsl_LIBS} -lm

noinst_PROGRAMS = ../test/dsim
___test_dsim_SOURCES = main.c ../lib/libdsim.la
___test_dsim_LDADD = ../lib/libdsim.la -lm -lrt
