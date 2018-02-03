/** @file
 *  @brief C-string module
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <libbds/bds_string.h>

#define MAX(a,b) ( (a) > (b) ? (a) : (b) )

static inline void append_tok( char *str, size_t *alloc_size, size_t *num_tok, char **(*tok) );

static char *__adjustl( char *str, size_t *str_len );

static char *__trim( char *str, size_t *str_len );

bool bds_string_contains( const char *str, const char *substr )
{
	return ( strstr( str, substr ) != NULL );
}

char *bds_string_adjustl( char *str )
{
	size_t str_len = strlen(str);
	return __adjustl( str, &str_len );
}

char *bds_string_adjustr( char *str )
{
	const size_t str_len = strlen(str);
	const char *const str_end = str + str_len;
	const char *c = str_end;

	while( c != str &&  *(c-1) == ' ' ) --c;

	const long long move_len = str_end - c;
	if( move_len == str_len ) {
		*str = '\0';
	} else if( move_len > 0 ) {
		memmove( str + move_len, str, str_len - move_len);
		memset( str, ' ', move_len);
	}

	return str;
}

char *bds_string_trim( char *str )
{
	size_t str_len = strlen(str);	
	return __trim( str, &str_len );
}

char *bds_string_atrim( char *str )
{
	size_t str_len = strlen( str );
	return __trim( __adjustl( str, &str_len ), &str_len );
}

void bds_string_tokenize( char *str, const char *delim,  size_t *num_tok, char **(*tok) )
{

	size_t alloc_tok = 0;
	*tok = NULL;
	*num_tok = 0;

	char *__str = str;

	while( ( str = strtok( __str, delim ) ) != NULL ) {
		// New string
		append_tok( str, &alloc_tok, num_tok, tok );
		__str = NULL;
	}
}

void bds_string_tokenize_w( char *str, const char *delim, size_t *num_tok, char **(*tok) )
{
	const char * const str_end = str + strlen(str);
	const size_t delim_len     = strlen(delim);
	
	size_t alloc_tok = 0;
	*tok = NULL;
	*num_tok = 0;

	char *__str = str;
	char *c;

	while( ( c = strchr( __str, delim[0] ) ) != NULL ) {

		if( str_end - c < delim_len )
			break;

		if( strncmp( c, delim, delim_len ) == 0 ) {
			// Do not include empty strings
			if( c - str > 0 ) {
				// New string
				*c = '\0';			
				append_tok( str, &alloc_tok, num_tok, tok );
			}
			__str = str = c + delim_len;
		} else {
			__str = c + 1;
		}
			
		if( __str >= str_end )
			break;
	}

	// Take care of trailing string
	if( *num_tok > 0 && str < str_end )
		append_tok( str, &alloc_tok, num_tok, tok );

}

char *bds_string_substr( const char *str, size_t len )
{
	if( len > strlen(str) )
		return NULL;	
			
	char *substr = malloc( len + 1 );
	//strncpy( substr, str, len );
	memcpy( substr, str, len );
	substr[len] = '\0';
	
	return substr;
}

char *bds_string_find( const char *str, const char *seq ) 
{
	return strstr( str, seq );
}

char *bds_string_rfind( const char *str, const char *seq )
{
      	const size_t seq_len = strlen(seq);
      	const char *s        = str + strlen(str) - seq_len;

	while( s >= str ) {
		while( *s != seq[0] ) {
			if( s == str ) break;
			--s;
		}

		if( strncmp( s, seq, seq_len ) == 0 ) {
			return (char *)s;
		}
		--s;
      	}

	return NULL;
}

bool bds_string_isnum(const char *str )
{
	const char * const str_end = str + strlen(str);
	while( str != str_end ) {
		if( ! isdigit(*str) )
			return false;
		++str;
	}
	return true;
}


char *bds_string_dup( const char *str )
{
	const size_t str_len = strlen( str );
	char *s = malloc( str_len + 1 );
	if( s ) {
		memcpy( s, str, str_len + 1 );
	}
	return s;
}
	
static inline void append_tok( char *str, size_t *alloc_size, size_t *num_tok, char **(*tok) )
{
	if( *num_tok == *alloc_size ) {
		*alloc_size = MAX( (*alloc_size) << 1, 2 );
		*tok = realloc( *tok, (*alloc_size) * sizeof(**tok) );
	}
	(*tok)[(*num_tok)++] = str;	
}

static char *__adjustl( char *str, size_t *str_len )
{
	const char *c = str;
	while( *c == ' ' ) ++c;
	
	long long move_len = c - str;
	if( move_len == *str_len ) { // Occurs when string is empty
		*str_len = 0;		
		*str = '\0';
	} else if( move_len > 0 ) {
		*str_len -= move_len;
		memmove( str, c, *str_len + 1); // Copy the null character
	}

	return str;
}

static char *__trim( char *str, size_t *str_len )
{
	char *c = str + (*str_len);
	while( c != str &&  *(c-1) == ' ' )
		--c;

	*str_len -= str + (*str_len) - c;
	*c = '\0';

	return str;

}
