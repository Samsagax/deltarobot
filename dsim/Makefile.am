AM_CPPFLAGS = \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	${gobject_CFLAGS} \
	${gsl_CFLAGS}

AM_CFLAGS = -std=gnu99

sources = \
	dsim.h \
	dsim_geometry.c \
	dsim_solver.c \
	dsim_trajectory.c \
	dsim_trajectory_joint.c \
	dsim_trajectory_linear.c \
	dsim_trajectory_command.c \
	dsim_trajectory_control.c \
	dsim_manipulator.c \
	dsim_dynamic_spec.c \
	dsim_dynamic_model.c

lib_LTLIBRARIES = ../lib/libdsim.la
___lib_libdsim_la_SOURCES = ${sources}
___lib_libdsim_la_LIBADD = ${gobject_LIBS} ${gsl_LIBS} -lm
