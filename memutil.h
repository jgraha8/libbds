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

static void *xalloc( size_t len )
{
	void *v = malloc( len );
	assert(v);
	memset( v, 0, len );
	return v;
}

static void *xrealloc( void *v, size_t len_old, size_t len )
{
	void *_v = realloc( v, len );
	assert( _v );
	if( len > len_old ) {
		memset( _v + len_old,
			0,
			len - len_old );
	}
	return _v;
}

static inline void xmemswap( void *a, void *b, size_t len )
{
	char tmp[len];
	
	memcpy( tmp, a  , len );
	memcpy( a  , b  , len );
	memcpy( b  , tmp, len );
}

#endif
