## Makefile.am -- Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS = -I m4

#SUBDIRS= dscs bspline lagrange psolver sparselu
SUBDIRS= \
	ckunkwurx \
	queue

#SUBLIBS = bspline/libdscsbspline.la lagrange/libdscslagrange.la psolver/libdscspsolver.la sparselu/libdscssparselu.la
SUBLIBS = \
	queue/libckuqueue.la

lib_LTLIBRARIES = libckunkwurx.la
libckunkwurx_la_SOURCES = version.c
libckunkwurx_la_LDFLAGS=-version-info 0:0:0
libckunkwurx_la_LIBADD = $(SUBLIBS)

pkginclude_HEADERS = cku_version.h

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
