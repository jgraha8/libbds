#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
noinst_LTLIBRARIES = libbdsstack.la

pkginclude_HEADERS = bds_stack.h

AM_CPPFLAGS = -I${top_srcdir}

libbdsstack_la_SOURCES = stack.c

check_PROGRAMS = test_stack
test_stack_SOURCES = test_stack.c 
test_stack_LDADD = $(top_builddir)/libbds.la

TESTS = $(check_PROGRAMS)

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
#libbspline_la_CFLAGS = -I$(top_srcdir)/include

#AM_FCFLAGS=@AM_FCFLAGS@

# These files will end up in the install include directory
# For example, /usr/include
#noinst_HEADERS = bspline.h 
