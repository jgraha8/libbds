## Makefile.am -- Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS = -I m4

#SUBDIRS= dscs bspline lagrange psolver sparselu
SUBDIRS= \
	libbds	\
	fsm	\
	heap	\
	list	\
	queue	\
	serialize \
	stack	\
	string \
	vector

#SUBLIBS = bspline/libdscsbspline.la lagrange/libdscslagrange.la psolver/libdscspsolver.la sparselu/libdscssparselu.la
SUBLIBS = \
	fsm/libbds_fsm.la \
	heap/libbds_heap.la \
	list/libbds_list.la \
	queue/libbds_queue.la \
	serialize/libbds_serialize.la \
	stack/libbds_stack.la \
	string/libbds_string.la \
	vector/libbds_vector.la

lib_LTLIBRARIES = libbds.la
libbds_la_SOURCES = version.c
#
# The rules of thumb, when dealing with these values are:
#
#    Always increase the revision value.
#
#    Increase the current value whenever an interface has been added, removed or changed.
#
#    Increase the age value only if the changes made to the ABI are backward compatible.
#
# See: https://autotools.io/libtool/version.html
#
# version-info current:revision:age
#
libbds_la_LDFLAGS=-version-info 13:16:12
libbds_la_LIBADD = $(SUBLIBS)

pkginclude_HEADERS = bds_version.h bds_modulus.h

noinst_HEADERS = memutil.h

#EXTRA_DIST = doxygen/doxygen.cfg

#MOSTLYCLEANFILES = $(DX_CLEANFILES)

# Documentation hooks in with the 'html' target
#html-local: doxygen-run

# # Target for all documentation
# docs: 
# 	cd bspline     && make docs && \
# 	cd ../psolver  && make docs && \
# 	cd ../sparselu && make docs

#include doxygen/doxygen.am

EXTRA_DIST = COPYING.LESSER README.md INSTALL doxygen/doxygen.cfg

MOSTLYCLEANFILES = $(DX_CLEANFILES)

# Documentation hooks in with the 'html' target
html-local: doxygen-run

# Target for all documentation
docs: doxygen-run

include doxygen/doxygen.am
