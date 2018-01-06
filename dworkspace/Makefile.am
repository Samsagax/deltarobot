AM_CPPFLAGS = -I$(top_builddir) \
			-I$(top_srcdir) \
			${glib_CFLAGS}

AM_CFLAGS = -std=c99 \
	${glib_CFLAGS}

LDADD = ${glib_LIBS} \
	../lib/libdsim.la

sources = main.c \
		../lib/libdsim.la

bin_PROGRAMS = ../test/dworkspace
___test_dworkspace_SOURCES = ${sources}
