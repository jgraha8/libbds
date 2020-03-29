#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
noinst_LTLIBRARIES = libbds_rbtree.la

pkginclude_HEADERS = bds_rbtree.h

AM_CFLAGS = -I${top_srcdir}

libbds_rbtree_la_SOURCES = rbtree.c

check_PROGRAMS = test_rbtree1
test_rbtree1_SOURCES = test_rbtree1.c
test_rbtree1_LDADD = $(top_builddir)/libbds.la

TESTS = $(check_PROGRAMS)

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
#libbspline_la_CFLAGS = -I$(top_srcdir)/include

#AM_FCFLAGS=@AM_FCFLAGS@

# These files will end up in the install include directory
# For example, /usr/include
#noinst_HEADERS = bspline.h 
