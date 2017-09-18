#ifndef __BDS_STRING_H__
#define __BDS_STRING_H__

#include <stdbool.h>
#include <unistd.h>

char *bds_string_dup( const char *str );

bool bds_string_contains( const char *str, const char *substr );

char *bds_string_find( const char *str, const char *seq );

char *bds_string_rfind( const char *str, const char *seq );

char *bds_string_adjustl( char *str );

char *bds_string_adjustr( char *str );

char *bds_string_trim( char *str );

char *bds_string_atrim( char *str );

void bds_string_tokenize( char *str, const char *delim,  size_t *num_tok, char **(*tok) );

void bds_string_wtokenize( char *str, const char *delim, size_t *num_tok, char **(*tok) );

char *bds_string_substr( const char *str, size_t len );

#endif // __BDS_STRING_H__
