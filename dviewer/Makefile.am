LDADD = -lm ${gl_LIBS} ${glu_LIBS} ${gtkglext_LIBS} ../dsim/libdsim.la
AM_CPPFLAGS = -I$(top_builddir) \
	-I$(top_srcdir) \
	${gl_CFLAGS} \
	${glu_CFLAGS} \
	${gtkglext_CFLAGS}
noinst_PROGRAMS = ../test/dviewer-gtk
___test_dviewer_gtk_SOURCES = main-gtk.c dviewer_engine.c ../dsim/libdsim.la
#___test_dviewer_gtk_LDADD = ${gl_LIBS} 
#			${glu_LIBS} \
#			${gtkglext_LIBS} \
#			-L../dsim -ldsim -lm -lm
