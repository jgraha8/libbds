#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libbds/bds_string.h>

#define YESNO(a) (a ? "YES" : "NO")

int main(int argc, char *argv[])
{
        const char *input_str   = "The red dog || jumps over the | big dog";
        const char *input_delim = "||";

        assert(bds_string_contains(input_str, input_delim));
        assert((bds_string_find(input_str, "dog") - input_str) == 8);
        assert((bds_string_rfind(input_str, "dog") - input_str) == 36);

        // Sub-string
        char *substr = bds_string_substr(input_str + 4, 6);
	assert( substr );
	assert( strlen(substr) == 6 );
	assert( strncmp(substr, input_str+4, 6) == 0 );
	free(substr);
	
        // Tokenize
        size_t num_tok;
        char **tok;

        char *str = bds_string_dup(input_str);
	assert( strlen(str) == strlen(input_str) );
	assert( strcmp(str,input_str) == 0 );
	
        bds_string_tokenize_w(str, input_delim, &num_tok, &tok);

	assert(num_tok == 2);	
	const char *atrim_wtok[] = { "The red dog",
				     "jumps over the | big dog" };
	
        for (size_t n = 0; n < num_tok; ++n) {
                bds_string_atrim(tok[n]);
		assert( strcmp(atrim_wtok[n], tok[n]) == 0 );
	}
        free(tok);
	free(str);

	str = bds_string_dup(input_str);	
        bds_string_tokenize(str, input_delim, &num_tok, &tok);

	assert(num_tok == 3);	
	const char *atrim_tok[] = { "The red dog",
				     "jumps over the",
				     "big dog" };
	

        for (size_t n = 0; n < num_tok; ++n) {
                bds_string_atrim(tok[n]);
		assert( strcmp(atrim_tok[n], tok[n]) == 0 );
        }

        free(tok);
	free(str);

        return 0;
}
