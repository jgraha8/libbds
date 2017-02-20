/** @file
 *  @brief Memory management utilities used by the ckunkwurx library
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#ifndef __MEMUTIL_H__
#define __MEMUTIL_H__

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void *xalloc( size_t n_elem, size_t elem_len )
{
	void *v = malloc( n_elem * elem_len );
	assert(v);
	memset( v, 0, n_elem * elem_len);
	return v;
}

static void *xrealloc( void *v, size_t n_elem_old, size_t n_elem, size_t elem_len )
{
	void *_v = realloc( v, n_elem * elem_len );
	assert( _v );
	if( n_elem > n_elem_old ) {
		memset( _v + n_elem_old * elem_len,
			0,
			(n_elem - n_elem_old)*elem_len );
	}
	return _v;
}

#endif
