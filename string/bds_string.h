/** @file
 *  @brief C-string module definitions
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#ifndef __BDS_STRING_H__
#define __BDS_STRING_H__

#include <stdbool.h>
#include <unistd.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

char *bds_string_dup(const char *str);

char *bds_string_dup_concat(int num_substr, const char *str, ...);

char *bds_string_prependf(char *dest, size_t max_len, const char *fmt, ...);
	
char *bds_string_concatf(char *dest, size_t max_len, const char *fmt, ...);

char *bds_string_copyf(char *dest, size_t max_len, const char *fmt, ... );

bool bds_string_contains(const char *str, const char *substr);

size_t bds_string_num_contains(const char *str, const char *substr);

char *bds_string_find(const char *str, const char *seq);

char *bds_string_rfind(const char *str, const char *seq);

char *bds_string_adjustl(char *str);

char *bds_string_adjustr(char *str);

char *bds_string_trim(char *str);

char *bds_string_atrim(char *str);

char *bds_string_toupper(char *s);

char *bds_string_tolower(char *s);	

void bds_string_tokenize(char *str, const char *delim, size_t *num_tok, char **(*tok));

void bds_string_tokenize_w(char *str, const char *delim, size_t *num_tok, char **(*tok));

char *bds_string_substr(const char *str, size_t len);

bool bds_string_isnum(const char *str);

wchar_t *bds_wstring_dup(const wchar_t *str);

wchar_t *bds_wstring_dup_concat(int num_substr, const wchar_t *str, ...);

wchar_t *bds_wstring_prependf(wchar_t *dest, size_t max_len, const wchar_t *fmt, ...);
	
wchar_t *bds_wstring_concatf(wchar_t *dest, size_t max_len, const wchar_t *fmt, ...);

wchar_t *bds_wstring_copyf(wchar_t *dest, size_t max_len, const wchar_t *fmt, ... );
	
bool bds_wstring_contains(const wchar_t *str, const wchar_t *substr);

size_t bds_wstring_num_contains(const wchar_t *str, const wchar_t *substr);

wchar_t *bds_wstring_find(const wchar_t *str, const wchar_t *seq);

wchar_t *bds_wstring_rfind(const wchar_t *str, const wchar_t *seq);

wchar_t *bds_wstring_adjustl(wchar_t *str);

wchar_t *bds_wstring_adjustr(wchar_t *str);

wchar_t *bds_wstring_trim(wchar_t *str);

wchar_t *bds_wstring_atrim(wchar_t *str);

wchar_t *bds_wstring_toupper(wchar_t *s);

wchar_t *bds_wstring_tolower(wchar_t *s);	

void bds_wstring_tokenize(wchar_t *str, const wchar_t *delim, size_t *num_tok, wchar_t **(*tok));

void bds_wstring_tokenize_w(wchar_t *str, const wchar_t *delim, size_t *num_tok, wchar_t **(*tok));

wchar_t *bds_wstring_substr(const wchar_t *str, size_t len);

bool bds_wstring_isnum(const wchar_t *str);

	
#ifdef __cplusplus
}
#endif

#ifdef BDS_NAMESPACE
#define string_dup bds_string_dup

#define string_dup_concat bds_string_dup_concat

#define string_prependf bds_string_prependf

#define string_concatf bds_string_concatf

#define string_copyf bds_string_copyf

#define string_contains bds_string_contains

#define string_num_contains bds_string_num_contains

#define string_find bds_string_find

#define string_rfind bds_string_rfind

#define string_adjustl bds_string_adjustl

#define string_adjustr bds_string_adjustr

#define string_trim bds_string_trim

#define string_atrim bds_string_atrim

#define string_toupper bds_string_toupper

#define string_tolower bds_string_tolower

#define string_tokenize bds_string_tokenize

#define string_tokenize_w bds_string_tokenize_w 

#define string_substr bds_string_substr

#define string_isnum bds_string_isnum

#define wstring_dup bds_wstring_dup

#define wstring_dup_concat bds_wstring_dup_concat

#define wstring_prependf bds_wstring_prependf

#define wstring_concatf bds_wstring_concatf

#define wstring_copyf bds_wstring_copyf

#define wstring_contains bds_wstring_contains

#define wstring_num_contains bds_wstring_num_contains

#define wstring_find bds_wstring_find

#define wstring_rfind bds_wstring_rfind

#define wstring_adjustl bds_wstring_adjustl

#define wstring_adjustr bds_wstring_adjustr

#define wstring_trim bds_wstring_trim

#define wstring_atrim bds_wstring_atrim

#define wstring_toupper bds_wstring_toupper

#define wstring_tolower bds_wstring_tolower

#define wstring_tokenize bds_wstring_tokenize

#define wstring_tokenize_w bds_wstring_tokenize_w 

#define wstring_substr bds_wstring_substr

#define wstring_isnum bds_wstring_isnum
#endif

#endif // __BDS_STRING_H__
