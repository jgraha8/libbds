#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
noinst_LTLIBRARIES = libbds_serialize.la

pkginclude_HEADERS = bds_serialize.h

AM_CFLAGS = -I${top_srcdir}

libbds_serialize_la_SOURCES = serialize.c

check_PROGRAMS = test_serialize1 test_serialize2 test_serialize3
test_serialize1_SOURCES = test_serialize1.c
test_serialize1_LDADD = $(top_builddir)/libbds.la

test_serialize2_SOURCES = test_serialize2.c
test_serialize2_LDADD = $(top_builddir)/libbds.la

test_serialize3_SOURCES = test_serialize3.c
test_serialize3_LDADD = $(top_builddir)/libbds.la

TESTS = $(check_PROGRAMS)
