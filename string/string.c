/** @file
 *  @brief C-string module
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#include <ctype.h>
#include <libbds/bds_string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

static inline void append_tok(char *str, size_t *alloc_size, size_t *num_tok, char **(*tok));
static inline void w_append_tok(wchar_t *str, size_t *alloc_size, size_t *num_tok, wchar_t **(*tok));

static char *__adjustl(char *str, size_t *str_len);
static wchar_t *__w_adjustl(wchar_t *str, size_t *str_len);

static char *__trim(char *str, size_t *str_len);
static wchar_t *__w_trim(wchar_t *str, size_t *str_len);

bool bds_string_contains(const char *str, const char *substr) { return (strstr(str, substr) != NULL); }

size_t bds_string_num_contains(const char *str, const char *substr)
{
        size_t num_contains     = 0;
        const size_t substr_len = strlen(substr);

        const char *s = str;
        while ((s = strstr(s, substr))) {
                ++num_contains;
                s += substr_len;
        }
        return num_contains;
}

bool bds_wstring_contains(const wchar_t *str, const wchar_t *substr) { return (wcsstr(str, substr) != NULL); }

size_t bds_wstring_num_contains(const wchar_t *str, const wchar_t *substr)
{
        size_t num_contains     = 0;
        const size_t substr_len = wcslen(substr);

        const wchar_t *s = str;
        while ((s = wcsstr(s, substr))) {
                ++num_contains;
                s += substr_len;
        }
        return num_contains;
}

char *bds_string_adjustl(char *str)
{
        size_t str_len = strlen(str);
        return __adjustl(str, &str_len);
}

wchar_t *bds_wstring_adjustl(wchar_t *str)
{
        size_t str_len = wcslen(str);
        return __w_adjustl(str, &str_len);
}

char *bds_string_adjustr(char *str)
{
        const size_t str_len      = strlen(str);
        const char *const str_end = str + str_len;
        const char *c             = str_end;

        while (c != str && *(c - 1) == ' ')
                --c;

        const long long move_len = str_end - c;
        if (move_len == str_len) {
                *str = '\0';
        } else if (move_len > 0) {
                memmove(str + move_len, str, str_len - move_len);
                memset(str, ' ', move_len);
        }

        return str;
}

wchar_t *bds_wstring_adjustr(wchar_t *str)
{
        const size_t str_len         = wcslen(str);
        const wchar_t *const str_end = str + str_len;
        const wchar_t *c             = str_end;

        while (c != str && *(c - 1) == ' ')
                --c;

        const long long move_len = str_end - c;
        if (move_len == str_len) {
                *str = '\0';
        } else if (move_len > 0) {
                memmove(str + sizeof(wchar_t) * move_len, str, sizeof(wchar_t) * (str_len - move_len));
                memset(str, ' ', sizeof(wchar_t) * move_len);
        }

        return str;
}

char *bds_string_trim(char *str)
{
        size_t str_len = strlen(str);
        return __trim(str, &str_len);
}

wchar_t *bds_wstring_trim(wchar_t *str)
{
        size_t str_len = wcslen(str);
        return __w_trim(str, &str_len);
}

char *bds_string_atrim(char *str)
{
        size_t str_len = strlen(str);
        return __trim(__adjustl(str, &str_len), &str_len);
}

wchar_t *bds_wstring_atrim(wchar_t *str)
{
        size_t str_len = wcslen(str);
        return __w_trim(__w_adjustl(str, &str_len), &str_len);
}

void bds_string_tokenize(char *str, const char *delim, size_t *num_tok, char **(*tok))
{

        size_t alloc_tok = 0;
        *tok             = NULL;
        *num_tok         = 0;

        char *__str = str;

        while ((str = strtok(__str, delim)) != NULL) {
                // New string
                append_tok(str, &alloc_tok, num_tok, tok);
                __str = NULL;
        }
}

void bds_wstring_tokenize(wchar_t *str, const wchar_t *delim, size_t *num_tok, wchar_t **(*tok))
{

        size_t alloc_tok = 0;
        *tok             = NULL;
        *num_tok         = 0;

        wchar_t *__str = str;
        wchar_t *next  = NULL;

        while ((str = wcstok(__str, delim, &next)) != NULL) {
                // New string
                w_append_tok(str, &alloc_tok, num_tok, tok);
                __str = NULL;
        }
}

void bds_string_tokenize_w(char *str, const char *delim, size_t *num_tok, char **(*tok))
{
        const char *const str_end = str + strlen(str);
        const size_t delim_len    = strlen(delim);

        size_t alloc_tok = 0;
        *tok             = NULL;
        *num_tok         = 0;

        char *__str = str;
        char *c;

        while ((c = strchr(__str, delim[0])) != NULL) {

                if (str_end - c < delim_len)
                        break;

                if (strncmp(c, delim, delim_len) == 0) {
                        // Do not include empty strings
                        if (c - str > 0) {
                                // New string
                                *c = '\0';
                                append_tok(str, &alloc_tok, num_tok, tok);
                        }
                        __str = str = c + delim_len;
                } else {
                        __str = c + 1;
                }

                if (__str >= str_end)
                        break;
        }

        // Take care of trailing string
        if (*num_tok > 0 && str < str_end)
                append_tok(str, &alloc_tok, num_tok, tok);
}

void bds_wstring_tokenize_w(wchar_t *str, const wchar_t *delim, size_t *num_tok, wchar_t **(*tok))
{
        const wchar_t *const str_end = str + wcslen(str);
        const size_t delim_len       = wcslen(delim);

        size_t alloc_tok = 0;
        *tok             = NULL;
        *num_tok         = 0;

        wchar_t *__str = str;
        wchar_t *c;

        while ((c = wcschr(__str, delim[0])) != NULL) {

                if (str_end - c < delim_len)
                        break;

                if (wcsncmp(c, delim, delim_len) == 0) {
                        // Do not include empty strings
                        if (c - str > 0) {
                                // New string
                                *c = '\0';
                                w_append_tok(str, &alloc_tok, num_tok, tok);
                        }
                        __str = str = c + delim_len;
                } else {
                        __str = c + 1;
                }

                if (__str >= str_end)
                        break;
        }

        // Take care of trailing string
        if (*num_tok > 0 && str < str_end)
                w_append_tok(str, &alloc_tok, num_tok, tok);
}

char *bds_string_substr(const char *str, size_t len)
{
        if (len > strlen(str))
                return NULL;

        char *substr = malloc(len + 1);
        // strncpy( substr, str, len );
        memcpy(substr, str, len);
        substr[len] = '\0';

        return substr;
}

wchar_t *bds_wstring_substr(const wchar_t *str, size_t len)
{
        if (len > wcslen(str))
                return NULL;

        wchar_t *substr = malloc(sizeof(wchar_t) * (len + 1));
        // strncpy( substr, str, len );
        memcpy(substr, str, sizeof(wchar_t) * len);
        substr[len] = '\0';

        return substr;
}

char *bds_string_find(const char *str, const char *seq) { return strstr(str, seq); }

wchar_t *bds_wstring_find(const wchar_t *str, const wchar_t *seq) { return wcsstr(str, seq); }

char *bds_string_rfind(const char *str, const char *seq)
{
        const size_t seq_len = strlen(seq);
        const char *s        = str + strlen(str) - seq_len;

        while (s >= str) {
                while (*s != seq[0]) {
                        if (s == str)
                                break;
                        --s;
                }

                if (strncmp(s, seq, seq_len) == 0) {
                        return (char *)s;
                }
                --s;
        }

        return NULL;
}

wchar_t *bds_wstring_rfind(const wchar_t *str, const wchar_t *seq)
{
        const size_t seq_len = wcslen(seq);
        const wchar_t *s     = str + wcslen(str) - seq_len;

        while (s >= str) {
                while (*s != seq[0]) {
                        if (s == str)
                                break;
                        --s;
                }

                if (wcsncmp(s, seq, seq_len) == 0) {
                        return (wchar_t *)s;
                }
                --s;
        }

        return NULL;
}

bool bds_string_isnum(const char *str)
{
        const char *const str_end = str + strlen(str);
        while (str != str_end) {
                if (!isdigit(*str))
                        return false;
                ++str;
        }
        return true;
}

bool bds_wstring_isnum(const wchar_t *str)
{
        const wchar_t *const str_end = str + wcslen(str);
        while (str != str_end) {
                if (!isdigit((unsigned char)*str))
                        return false;
                ++str;
        }
        return true;
}

char *bds_string_dup(const char *str)
{
        const size_t str_len = strlen(str);
        char *s              = malloc(str_len + 1);
        if (s) {
                memcpy(s, str, str_len + 1);
        }
        return s;
}

wchar_t *bds_wstring_dup(const wchar_t *str)
{
        const size_t str_len = wcslen(str);
        wchar_t *s           = malloc(sizeof(wchar_t) * (str_len + 1));
        if (s) {
                memcpy(s, str, sizeof(wchar_t) * (str_len + 1));
        }
        return s;
}

char *bds_string_dup_concat(int num_str, const char *str, ...)
{
        va_list ap;

        size_t str_len_tbl[num_str];
        const char *str_tbl[num_str];
        size_t str_len = 0;

        str_tbl[0]     = str;
        str_len_tbl[0] = strlen(str_tbl[0]);
        str_len        = str_len_tbl[0];

        va_start(ap, str);
        for (int i = 1; i < num_str; ++i) {
                str_tbl[i]     = va_arg(ap, const char *);
                str_len_tbl[i] = strlen(str_tbl[i]);
                str_len += str_len_tbl[i];
        }
        va_end(ap);

        char *str_dup = malloc(str_len + 1);

        if (!str_dup)
                return NULL;

        char *s = str_dup;

        for (int i = 0; i < num_str; ++i) {
                memcpy(s, str_tbl[i], str_len_tbl[i] + 1); // Copy null terminator
                s += str_len_tbl[i];                       // Advance to the null terminator
        }

        return str_dup;
}

wchar_t *bds_wstring_dup_concat(int num_str, const wchar_t *str, ...)
{
        va_list ap;

        size_t str_len_tbl[num_str];
        const wchar_t *str_tbl[num_str];
        size_t str_len = 0;

        str_tbl[0]     = str;
        str_len_tbl[0] = wcslen(str_tbl[0]);
        str_len        = str_len_tbl[0];

        va_start(ap, str);
        for (int i = 1; i < num_str; ++i) {
                str_tbl[i]     = va_arg(ap, const wchar_t *);
                str_len_tbl[i] = wcslen(str_tbl[i]);
                str_len += str_len_tbl[i];
        }
        va_end(ap);

        wchar_t *str_dup = malloc(sizeof(wchar_t) * (str_len + 1));

        if (!str_dup)
                return NULL;

        wchar_t *s = str_dup;

        for (int i = 0; i < num_str; ++i) {
                memcpy(s, str_tbl[i], sizeof(wchar_t) * (str_len_tbl[i] + 1)); // Copy null terminator
                s += str_len_tbl[i];                                           // Advance to the null terminator
        }

        return str_dup;
}

char *bds_string_concatf(char *dest, size_t max_len, const char *fmt, ...)

{
        size_t __offset  = strlen(dest);
        size_t __max_len = (max_len >= __offset ? max_len - __offset : 0);

	va_list va;
	va_start(va, fmt);
        vsnprintf(dest + __offset, __max_len, fmt, va);
	va_end(va);
	
        return dest;
}

wchar_t *bds_wstring_concatf(wchar_t *dest, size_t max_len, const wchar_t *fmt, ...)

{
        size_t __offset  = wcslen(dest);
        size_t __max_len = (max_len >= __offset ? max_len - __offset : 0);

	va_list va;
	va_start(va, fmt);
        vswprintf(dest + __offset, __max_len, fmt, va);
	va_end(va);
	
        return dest;
}

static inline void append_tok(char *str, size_t *alloc_size, size_t *num_tok, char **(*tok))
{
        if (*num_tok == *alloc_size) {
                *alloc_size = MAX((*alloc_size) << 1, 2);
                *tok        = realloc(*tok, (*alloc_size) * sizeof(**tok));
        }
        (*tok)[(*num_tok)++] = str;
}

static inline void w_append_tok(wchar_t *str, size_t *alloc_size, size_t *num_tok, wchar_t **(*tok))
{
        if (*num_tok == *alloc_size) {
                *alloc_size = MAX((*alloc_size) << 1, 2);
                *tok        = realloc(*tok, (*alloc_size) * sizeof(**tok));
        }
        (*tok)[(*num_tok)++] = str;
}

static char *__adjustl(char *str, size_t *str_len)
{
        const char *c = str;
        while (*c == ' ')
                ++c;

        long long move_len = c - str;
        if (move_len == *str_len) { // Occurs when string is empty
                *str_len = 0;
                *str     = '\0';
        } else if (move_len > 0) {
                *str_len -= move_len;
                memmove(str, c, *str_len + 1); // Copy the null character
        }

        return str;
}

static wchar_t *__w_adjustl(wchar_t *str, size_t *str_len)
{
        const wchar_t *c = str;
        while (*c == ' ')
                ++c;

        long long move_len = c - str;
        if (move_len == *str_len) { // Occurs when string is empty
                *str_len = 0;
                *str     = '\0';
        } else if (move_len > 0) {
                *str_len -= move_len;
                memmove(str, c, sizeof(wchar_t) * (*str_len + 1)); // Copy the null character
        }

        return str;
}

static char *__trim(char *str, size_t *str_len)
{
        char *c = str + (*str_len);
        while (c != str && *(c - 1) == ' ')
                --c;

        *str_len -= str + (*str_len) - c;
        *c = '\0';

        return str;
}

static wchar_t *__w_trim(wchar_t *str, size_t *str_len)
{
        wchar_t *c = str + (*str_len);
        while (c != str && *(c - 1) == ' ')
                --c;

        *str_len -= str + (*str_len) - c;
        *c = '\0';

        return str;
}
