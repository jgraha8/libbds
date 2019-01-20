# libbds

Copyright (C) 2017-2019 Jason Graham <jgraham@compukix.net>

Copying and distribution of this file, with or without modification,
are permitted in any medium without royalty provided the copyright
notice and this notice are preserved.  This file is offered as-is,
without any warranty.

## Overview

libbds is the "basic data structure" C library. It provides support for data
structures typically needed for C applications. In addition, there is support
for string manipulation, data serialization, and others.

The library is written using generic data types, which makes it flexible for any
data type. This excludes the string manipulation module, which is predicated on
standard, null-character terminated strings.

## Usage

To include support for a particular module in your source code, include the
respective header file(s) as needed. For example, to use the vector and string
module in your source file foo.c, add:

  #include <libbds/bds_vector.h>
  #include <libbds/bds_string.h>

in foo.c and then link with -lbds.

## API

The API is documented using doxygen. To generate the documentation in the 'docs'
directory, run 'make docs' after configuring and building the library.
