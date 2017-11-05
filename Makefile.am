## Makefile.am -- Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS = -I m4

#SUBDIRS= dscs bspline lagrange psolver sparselu
SUBDIRS= \
	libbds	\
	list	\
	queue	\
	stack	\
	string

#SUBLIBS = bspline/libdscsbspline.la lagrange/libdscslagrange.la psolver/libdscspsolver.la sparselu/libdscssparselu.la
SUBLIBS = \
	list/libbdslist.la \
	queue/libbdsqueue.la \
	stack/libbdsstack.la \
	string/libbdsstring.la

lib_LTLIBRARIES = libbds.la
libbds_la_SOURCES = version.c
libbds_la_LDFLAGS=-version-info 0:0:0
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

EXTRA_DIST = doxygen/doxygen.cfg

MOSTLYCLEANFILES = $(DX_CLEANFILES)

# Documentation hooks in with the 'html' target
html-local: doxygen-run

# Target for all documentation
docs: doxygen-run

include doxygen/doxygen.am
