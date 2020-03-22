#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

dnl AC_PREREQ([2.59])
AC_INIT([libbds], [0.13.0], [jgraham@compukix.net])

AC_CONFIG_AUX_DIR([.])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign])

## AM_MAINTAINER_MODE turns off "rebuild rules" that contain dependencies
## for Makefiles, configure, src/H5config.h, etc.  If AM_MAINTAINER_MODE
## is *not* included here, these files will be rebuilt if out of date.
## This is a problem because if users try to build on a machine with
## the wrong versions of autoconf and automake, these files will be
## rebuilt with the wrong versions and bad things can happen.
## Also, CVS doesn't preserve dependencies between timestamps, so
## Makefiles will often think rebuilding needs to occur when it doesn't.
## Developers should './configure --enable-maintainer-mode' to turn on
## rebuild rules.
AM_MAINTAINER_MODE

AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_MAKE_SET
AC_LANG(C)
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
#AC_PROG_FC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S

# Add C99 flag (required)
#CFLAGS="${CFLAGS} -std=c99"

dnl Initialize Libtool
LT_INIT

dnl Check if Libtool is present
dnl Libtool is used for building share libraries 
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([m],[cos])
#AC_CHECK_LIB([gslcblas],[cblas_dgemm])
#AC_CHECK_LIB([gsl],[gsl_blas_dgemm])

# Checks for header files.
AC_CHECK_HEADERS([ assert.h \
		   ctype.h \
		   math.h \
		   malloc.h \
		   stdbool.h \
		   stdio.h \
		   stdlib.h \
		   string.h \
		   stdint.h \
		   stdarg.h \
		   wchar.h \
		   wctype.h])
#AC_CHECK_HEADERS([gsl/gsl_errno.h gsl/gsl_spline.h])

# Process options
dnl Example of default-disabled feature
AC_ARG_ENABLE([optimized_mod],
    AS_HELP_STRING([--disable-optimized-mod], [disables power-of-two allocation for relevant vectors]))

AS_IF([test "x$disable_optimized_mod" = "xyes"],
      [AC_SUBST([BDS_USE_OPTIMIZED_MOD],[0])],
      [AC_SUBST([BDS_USE_OPTIMIZED_MOD],[1])])

AC_ARG_ENABLE([header_prefix],
    AS_HELP_STRING([--enable-header-prefix=DIR], [set the prefix for libbds header paths]),
    [],
    [enable_header_prefix=no])
    
case $enable_header_prefix in
  no)
	AC_SUBST([AM_HEADER_PREFIX], [""])
	AC_SUBST([BDS_HEADER_PREFIX], ["libbds"])
	;;
  *)
	AC_SUBST([AM_HEADER_PREFIX], ["$enable_header_prefix"])  
	AC_SUBST([BDS_HEADER_PREFIX], ["$enable_header_prefix/libbds"])
	;;
esac	

dnl ## Make sure flags are set to something (otherwise macros may set them later).
dnl AM_CFLAGS="${AM_CFLAGS}"
dnl AM_CXXFLAGS="${AM_CXXFLAGS}"
dnl AM_FCFLAGS="${AM_FCFLAGS}"
dnl AM_CPPFLAGS="${AM_CPPFLAGS}"
dnl AM_LDFLAGS="${AM_LDFLAGS}"
dnl CFLAGS="${CFLAGS}"
dnl CXXFLAGS="${CXXFLAGS}"
dnl FCFLAGS="${FCFLAGS}"
dnl CPPFLAGS="${CPPFLAGS}"
dnl LDFLAGS="${LDFLAGS}"

dnl AC_SUBST([AM_CFLAGS])
dnl AC_SUBST([AM_CXXFLAGS])
dnl AC_SUBST([AM_CPPFLAGS])
dnl AC_SUBST([AM_FCFLAGS])
dnl AC_SUBST([AM_LDFLAGS])

dnl ## Configure may need to alter any of the *FLAGS variables in order for
dnl ## various checks to work correctly. Save the user's value here so it
dnl ## can be restored once all configure checks are complete.
dnl saved_user_CFLAGS="$CFLAGS"
dnl saved_user_CXXFLAGS="$CXXFLAGS"
dnl saved_user_FCFLAGS="$FCFLAGS"
dnl saved_user_LDFLAGS="$LDFLAGS"
dnl saved_user_CPPFLAGS="$CPPFLAGS"

dnl ## Different compilers may need default libraries. They are specified in
dnl ## the config/* files, so we put this statement here so that it'll be
dnl ## set by the code which follows...
dnl ##
dnl DEFAULT_LIBS="-lm"

dnl ## Set default settings
dnl if [ test "x$AM_FCFLAGS" = "x" ]; then
dnl   AM_FCFLAGS="-I/usr/local/include -I/usr/include"
dnl else
dnl   AM_FCFLAGS="$AM_FCFLAGS -I/usr/local/include -I/usr/include"	
dnl fi

dnl ## ----------------------------------------------------------------------
dnl ## Some built-in configure checks can only see CFLAGS (not AM_CFLAGS), so
dnl ## we need to add this in so configure works as intended. We will need to
dnl ## reset this value at the end of configure, to preserve the user's settings.
dnl CFLAGS="${AM_CFLAGS} ${CFLAGS}"
dnl FCFLAGS="${AM_FCFLAGS} ${FCFLAGS}"
dnl CXXFLAGS="${AM_CXXFLAGS} ${CXXFLAGS}"
dnl CPPFLAGS="${AM_CPPFLAGS} ${CPPFLAGS}"
dnl LDFLAGS="${AM_LDFLAGS} ${LDFLAGS}"

dnl ## ----------------------------------------------------------------------
dnl ## Restore user's CFLAGS and company.
dnl CFLAGS="$saved_user_CFLAGS"
dnl FCFLAGS="$saved_user_FCFLAGS"
dnl CXXFLAGS="$saved_user_CXXFLAGS"
dnl CPPFLAGS="$saved_user_CPPFLAGS"
dnl LDFLAGS="$saved_user_LDFLAGS"

dnl # Documentation generation
DX_DOXYGEN_FEATURE([ON])
DX_HTML_FEATURE([ON])
DX_CHM_FEATURE([OFF])
DX_CHI_FEATURE([OFF])
DX_MAN_FEATURE([ON])
DX_RTF_FEATURE([OFF])
DX_XML_FEATURE([OFF])
DX_PDF_FEATURE([ON])
DX_PS_FEATURE([OFF])
DX_INIT_DOXYGEN([libbds],[doxygen/doxygen.cfg],[docs])


AC_CONFIG_FILES([	\
Makefile		\
libbds/Makefile		\
fsm/Makefile		\
heap/Makefile		\
list/Makefile		\
queue/Makefile 		\
queue/bds_queue.h	\
serialize/Makefile	\
stack/Makefile		\
stack/bds_stack.h	\
string/Makefile		\
vector/Makefile		\
vector/bds_vector.h	\
bds_modulus.h		\
])


AC_OUTPUT
