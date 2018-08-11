#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.60])
AC_INIT([DSim], [0.1])
AC_CONFIG_AUX_DIR([m4])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
#AC_CONFIG_SRCDIR([dsim/main.c])
AM_PROG_AR
LT_INIT

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for library functions.
PKG_CHECK_MODULES([gobject], [gobject-2.0])
PKG_CHECK_MODULES([gl], [gl])
PKG_CHECK_MODULES([glu], [glu])
PKG_CHECK_MODULES([glib], [glib-2.0])
PKG_CHECK_MODULES([gio], [gio-2.0])
#PKG_CHECK_MODULES([gthread], [gthread-2.0])
PKG_CHECK_MODULES([gtk], [gtk+-2.0])
PKG_CHECK_MODULES([gtkglext], [gtkglext-1.0])
#PKG_CHECK_MODULES([gtkdatabox], [gtkdatabox])
PKG_CHECK_MODULES([gsl], [gsl])

AC_FUNC_ERROR_AT_LINE

AC_CONFIG_FILES([Makefile
                 dsim/Makefile
                 dworkspace/Makefile
                 dviewer/Makefile])
#                 dynviewer/Makefile])
AC_OUTPUT
