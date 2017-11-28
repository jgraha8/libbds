#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
noinst_LTLIBRARIES = libbdslist.la

pkginclude_HEADERS = bds_list.h

AM_CFLAGS = -I${top_srcdir}

libbdslist_la_SOURCES = list.c

check_PROGRAMS = test_list1 test_list2 test_list3 test_list4

test_list1_SOURCES = test_list1.c
test_list1_LDADD = $(top_builddir)/libbds.la

test_list2_SOURCES = test_list2.c
test_list2_LDADD = $(top_builddir)/libbds.la

test_list3_SOURCES = test_list3.c
test_list3_LDADD = $(top_builddir)/libbds.la

test_list4_SOURCES = test_list4.c
test_list4_LDADD = $(top_builddir)/libbds.la

TESTS = $(check_PROGRAMS)

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
#libbspline_la_CFLAGS = -I$(top_srcdir)/include

#AM_FCFLAGS=@AM_FCFLAGS@

# These files will end up in the install include directory
# For example, /usr/include
#noinst_HEADERS = bspline.h 
