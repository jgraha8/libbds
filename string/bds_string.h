#ifndef __BDS_STRING_H__
#define __BDS_STRING_H__

bool bds_string_contains( const char *str, const char *substr );

char *bds_string_adjustl( char *str );

char *bds_string_trim( char *str );

void bds_string_tokenize( char *str, const char *delim,  size_t *num_tok, char **(*tok) );

void bds_string_tokenize_w( char *str, const char *delim, size_t *num_tok, char **(*tok) );

char *bds_string_substr( const char *str, size_t pos, size_t len );

#endif // __BDS_STRING_H__
