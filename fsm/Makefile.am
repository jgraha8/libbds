#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
noinst_LTLIBRARIES = libbds_fsm.la

pkginclude_HEADERS = bds_fsm.h

AM_CFLAGS = -I${top_srcdir}

libbds_fsm_la_SOURCES = fsm.c

check_PROGRAMS = test_fsm
test_fsm_SOURCES = test_fsm.c
test_fsm_LDADD = $(top_builddir)/libbds.la

TESTS = $(check_PROGRAMS)

