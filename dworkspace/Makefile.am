AM_CPPFLAGS = -I$(top_builddir) \
			-I$(top_srcdir) \
			${glib_CFLAGS} \
			${gobject_CFLAGS} \
			${gio_LIBS} \
			${gsl_CFLAGS}

AM_CFLAGS = -std=c99 \
		${gsl_CFLAGS} \
		${gobject_CFLAGS} \
		${glib_CFLAGS} \
		${gio_CFLAGS}

LDADD = ${glib_LIBS} \
	${gsl_LIBS} \
	${gobject_LIBS} \
	${gio_LIBS} \
	../lib/libdsim.la

sources = main.c \
		../lib/libdsim.la

bin_PROGRAMS = ../test/dworkspace
___test_dworkspace_SOURCES = ${sources}
