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

char *bds_string_concatf(char *dest, size_t max_len, const char *fmt, ...); 

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

wchar_t *bds_wstring_concatf(wchar_t *dest, size_t max_len, const wchar_t *fmt, ...);

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

#endif // __BDS_STRING_H__
