#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
noinst_LTLIBRARIES = libbds_heap.la

pkginclude_HEADERS = bds_heap.h
noinst_HEADERS = heap_util.h

AM_CFLAGS = -I${top_srcdir}

libbds_heap_la_SOURCES = heap.c

check_PROGRAMS = test_heap1 test_heap2 test_heap3
test_heap1_SOURCES = test_heap1.c
test_heap1_LDADD = $(top_builddir)/libbds.la

test_heap2_SOURCES = test_heap2.c
test_heap2_LDADD = $(top_builddir)/libbds.la

test_heap3_SOURCES = test_heap3.c
test_heap3_LDADD = $(top_builddir)/libbds.la

TESTS = $(check_PROGRAMS)

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
#libbspline_la_CFLAGS = -I$(top_srcdir)/include

#AM_FCFLAGS=@AM_FCFLAGS@

# These files will end up in the install include directory
# For example, /usr/include
#noinst_HEADERS = bspline.h 
