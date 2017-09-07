#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX(a,b) ( (a) > (b) ? (a) : (b) )

bool bds_string_contains( const char *str, const char *substr )
{
	return ( strstr( str, substr ) != NULL );
}


char *bds_string_adjustl( char *str )
{
	const size_t str_len = strlen(str);

	char *c = str;
	while( *c == ' ' ) ++c;
	
	long long move_len = c - str;
	
	if( move_len == str_len ) { // Occurs when string is empty
		*str = '\0';
	} else {
		memmove( str, c, str_len - move_len + 1); // Copy the null character
	}

	return str;
}

char *bds_string_trim( char *str )
{
	char *c = str + strlen(str);
	while( c != str &&  *(c-1) == ' ' ) --c;
	*c = '\0';

	return str;
}

static inline void append_tok( char *str, size_t *alloc_size, size_t *num_tok, char **(*tok) )
{
	if( *num_tok == *alloc_size ) {
		*alloc_size = MAX( (*alloc_size) << 1, 2 );
		*tok = realloc( *tok, (*alloc_size) * sizeof(**tok) );
	}
	(*tok)[(*num_tok)++] = str;	
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
			// New string
			*c = '\0';			
			append_tok( str, &alloc_tok, num_tok, tok );
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

char *bds_string_substr( const char *str, size_t pos, size_t len )
{

	const size_t str_len = strlen(str);
	
	if( pos + len > str_len )
		return NULL;	
			
	char *substr = malloc( len + 1 );
	strncpy( substr, str + pos, len );
	substr[len] = '\0';
	
	return substr;
}


int main(int argc, char *argv[] )
{
	if( argc != 3 ) {
		fprintf(stderr, "Usage: %s string arg\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	//bds_string_trim( bds_string_adjustl( argv[2] ) );
	
	printf("%s contains %s = %d\n",
	       argv[1], argv[2],
	       bds_string_contains(argv[1], argv[2]));
	
	// Sub-string
	char *substr = bds_string_substr( argv[1], 4, 6 );
	if( substr ) {
		printf("sub-string %s\n", substr );
	}
	
	// Tokenize
	size_t num_tok;
	char **tok;

	bds_string_tokenize_w( argv[1], argv[2], &num_tok, &tok );

	for( size_t n=0; n<num_tok; ++n ) {
		bds_string_trim( bds_string_adjustl( tok[n] ) );		
		printf("%s\n", tok[n]);
	}

	free(tok);

	
	return 0;
}
	
