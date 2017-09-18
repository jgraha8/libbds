#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <libbds/bds_string.h>

#define YESNO( a ) ( a ? "YES" : "NO" )

int main(int argc, char *argv[] )
{
	if( argc != 3 ) {
		fprintf(stderr, "Usage: %s string arg\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	//bds_string_trim( bds_string_adjustl( argv[2] ) );

 	//bds_string_adjustr( argv[1] );
	
	printf("\"%s\" contains \"%s\" = %s\n",
	       argv[1], argv[2],
	       YESNO(bds_string_contains(argv[1], argv[2])));

	printf("finding first position of dog in \"%s\" = %zd\n",
	       argv[1], bds_string_find( argv[1], "dog" ) - argv[1] );

	printf("finding last position of dog in \"%s\" = %zd\n",
	       argv[1], bds_string_rfind( argv[1], "dog" ) - argv[1] );

	// Sub-string
	char *substr = bds_string_substr( argv[1] + 4, 6 );
	if( substr ) {
		printf("sub-string \"%s\" (%zd)\n", substr, strlen(substr));
		free( substr );
	}
	
	// Tokenize
	size_t num_tok;
	char **tok;

	char *str = bds_string_dup( argv[1] );
	bds_string_wtokenize( argv[1], argv[2], &num_tok, &tok );

	for( size_t n=0; n<num_tok; ++n ) {
		//bds_string_adjustr( tok[n] );
		bds_string_atrim( tok[n] );
		printf("%s\n", tok[n]);
	}

	free(tok);

	printf("========\n");
	bds_string_tokenize( str, argv[2], &num_tok, &tok );

	for( size_t n=0; n<num_tok; ++n ) {
		//bds_string_adjustr( tok[n] );
		bds_string_atrim( tok[n] );
		printf("%s\n", tok[n]);
	}

	free(tok);

	
	return 0;
}
