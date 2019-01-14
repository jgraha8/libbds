/*
 * C-string module for wide-characters
 *
 * Copyright (C) 2018 Jason Graham <jgraham@compukix.net>
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
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libbds/bds_string.h>

#define YESNO(a) (a ? "YES" : "NO")

int main(int argc, char *argv[])
{
        const wchar_t *input_str   = L"The red dog || jumps over the | big dog";
        const wchar_t *input_delim = L"||";

	assert(bds_wstring_num_contains(input_str, L"dog") == 2 );
        assert(bds_wstring_contains(input_str, input_delim));
        assert((bds_wstring_find(input_str, L"dog") - input_str) == 8);
        assert((bds_wstring_rfind(input_str, L"dog") - input_str) == 36);

        // Sub-string
        wchar_t *substr = bds_wstring_substr(input_str + 4, 6);
	assert( substr );
	assert( wcslen(substr) == 6 );
	assert( wcsncmp(substr, input_str+4, 6) == 0 );
	free(substr);
	
        // Tokenize
        size_t num_tok;
        wchar_t **tok;

        wchar_t *str = bds_wstring_dup(input_str);
	assert( wcslen(str) == wcslen(input_str) );
	assert( wcscmp(str,input_str) == 0 );
	
        bds_wstring_tokenize_w(str, input_delim, &num_tok, &tok);

	assert(num_tok == 2);	
	const wchar_t *atrim_wtok[] = { L"The red dog",
				     L"jumps over the | big dog" };

	wchar_t *str_dup = bds_wstring_dup_concat( 2, atrim_wtok[0], atrim_wtok[1] );
	assert( wcscmp( str_dup, L"The red dogjumps over the | big dog") == 0 );

	memset(str_dup, 0, sizeof(*str_dup));
	
	bds_wstring_concatf(str_dup, 100, L"%ls%ls", atrim_wtok[0], atrim_wtok[1]);
	//fwprintf(stderr, L"%s\n", str_dup);
	assert(wcscmp( str_dup, L"The red dogjumps over the | big dog") == 0 );

	bds_wstring_copyf(str_dup, 100, L"%ls%ls", atrim_wtok[0], atrim_wtok[1]);
	assert(wcscmp( str_dup, L"The red dogjumps over the | big dog") == 0 );	
	
	assert(wcscmp( bds_wstring_tolower(str_dup), L"the red dogjumps over the | big dog") == 0 );
	assert(wcscmp( bds_wstring_toupper(str_dup), L"THE RED DOGJUMPS OVER THE | BIG DOG") == 0 );	

	bds_wstring_copyf(str_dup, 100, L"%ls", atrim_wtok[1]);
	bds_wstring_prependf(str_dup, 100, L"%ls", atrim_wtok[0]);

	assert(wcscmp( str_dup, L"The red dogjumps over the | big dog") == 0 );

	bds_wstring_remove_substr(str_dup, L"dog");
	//wprintf(L"%s\n", str_dup);
	assert( wcscmp( str_dup, L"The red jumps over the | big ") == 0 );

	free(str_dup);
	
        for (size_t n = 0; n < num_tok; ++n) {
                bds_wstring_atrim(tok[n]);
		assert( wcscmp(atrim_wtok[n], tok[n]) == 0 );
	}
        free(tok);
	free(str);

	str = bds_wstring_dup(input_str);	
        bds_wstring_tokenize(str, input_delim, &num_tok, &tok);

	assert(num_tok == 3);	
	const wchar_t *atrim_tok[] = { L"The red dog",
				     L"jumps over the",
				     L"big dog" };
	

        for (size_t n = 0; n < num_tok; ++n) {
                bds_wstring_atrim(tok[n]);
		assert( wcscmp(atrim_tok[n], tok[n]) == 0 );
        }

	assert(bds_wstring_isnum(L"12345"));
	assert(!bds_wstring_isnum(L"123A5"));
	assert(!bds_wstring_isnum(L" "));
	assert(bds_wstring_isnum(L"1"));
	assert(!bds_wstring_isnum(L"1 0 1"));	
	assert(!bds_wstring_isnum(L"!2345"));
        free(tok);
	free(str);

        return 0;
}
