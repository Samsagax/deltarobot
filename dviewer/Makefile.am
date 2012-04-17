AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) 
#noinst_PROGRAMS = ../test/dviewer ../test/dviewer-gtk
noinst_PROGRAMS = ../test/dviewer
___test_dviewer_SOURCES = main.c ../dsim/libdsim.la
___test_dviewer_LDADD = ${glu_LIBS} -lglut -L../dsim/ -ldsim

#___test_dviewer_gtk_SOURCES = main-gtk.c ../dsim/libdsim.la
#___test_dviewer_gtk_CFLAGS = ${gtk2_CFLAGS}
#___test_dviewer_gtk_LDADD = ${gtk2_LIBS} -L../dsim/ -ldsim
