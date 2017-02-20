#include <config.h>
#include <cku_version.h>

/* This file needs to use the top-level <gsl_version.h> due to the
   possibility of a VPATH-style build where the original source
   tree is on read-only filesystem and so will not be picked up
   by the symlinking comands in gsl/Makefile.am */


const char * cku_version = VERSION;


