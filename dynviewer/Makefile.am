AM_CPPFLAGS = -I$(top_builddir) \
			-I$(top_srcdir) \
			${gtk_CFLAGS} \
			${gtkdatabox_CFLAGS}

AM_CFLAGS = -std=c99

LDADD = ${gtk_LIBS} \
		${gtkdatabox_LIBS} \
		../lib/libdsim.la -lm

sources = main-gtk.c \
		  ../lib/libdsim.la

bin_PROGRAMS = ../test/dynviewer-gtk
___test_dynviewer_gtk_SOURCES = ${sources}
