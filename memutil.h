/**
 * @file
 * @brief Memory management utilities used by the libbds library
 *
 * Copyright (C) 2017-2018,2020 Jason Graham <jgraham@compukix.net>
 *
 * This file is part of libbds.
 *
 * libbds is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * libbds is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libbds. If not, see
 * <https://www.gnu.org/licenses/>.
 */
#ifndef __MEMUTIL_H__
#define __MEMUTIL_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((unused)) static void *xalloc(size_t len)
{
        void *v = malloc(len);
        assert(v);
        memset(v, 0, len);
        return v;
}

__attribute__((unused)) static void *xalloc_align(size_t alignment, size_t len)
{
        len     = alignment * ((len + alignment - 1) / alignment);
	void *v = NULL;
#if __linux__
  #if defined(_ISOC11_SOURCE)
        v = aligned_alloc(alignment, len);
  #else 
    #if( _POSIX_C_SOURCE >= 200112L)
	posix_memalign(&v, alignment, len);
    #else
      #ifdef HAVE_MALLOC_H
	v = memalign(alignment, len);
      #else
        #error "No aligned memory allocation function available"
      #endif
    #endif
  #endif
#elif __APPLE__ && __MACH__
	posix_memalign(&v, alignment, len);
#endif
        assert(v);
        memset(v, 0, len);
        return v;
}

__attribute__((unused)) static void *xrealloc(void *v, size_t len_old, size_t len)
{
        void *_v = realloc(v, len);
        assert(_v);
        if (len > len_old) {
                memset(_v + len_old, 0, len - len_old);
        }
        return _v;
}

__attribute__((unused)) static inline void xmemswap(void *a, void *b, size_t len, void *buf)
{
        memcpy(buf, a, len);
        memcpy(a, b, len);
        memcpy(b, buf, len);
}

#ifdef __cplusplus
}
#endif

#endif
