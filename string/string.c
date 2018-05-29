/** @file
 *  @brief C-string module
 *
 *  @author Jason Graham <jgraha8@gmail.com>
 */

#define _POSIX_C_SOURCE 1

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <wctype.h>
#include <stdarg.h>

#include <libbds/bds_string.h>

#define MAX(a,b) ( (a) > (b) ? (a) : (b) )

#define F_STRING(func) func(string, str, char)
#define F_WSTRING(func) func(wstring, wcs, wchar_t )

#define L_STRING(func) func(str, char)
#define L_WSTRING(func) func(wcs, wchar_t)

#define APPEND_TOK(S,C)							\
	static inline void __##S##append_tok( C *str, size_t *alloc_size, size_t *num_tok, C **(*tok) ) \
	{								\
		if( *num_tok == *alloc_size ) {				\
			*alloc_size = MAX( (*alloc_size) << 1, 2 );	\
			*tok = realloc( *tok, (*alloc_size) * sizeof(**tok) ); \
		}							\
		(*tok)[(*num_tok)++] = str;				\
	}

L_STRING(APPEND_TOK);
L_WSTRING(APPEND_TOK);

#define append_tok(S) __##S##append_tok


#define ADJUSTL(S,C)							\
	static C *__##S##adjustl( C *str, size_t *str_len )		\
	{								\
		const C *c = str;					\
		while( *c == ' ' ) ++c;					\
									\
		long long move_len = c - str;				\
		if( move_len == *str_len ) { /* Occurs when string is empty */ \
			*str_len = 0;					\
			*str = '\0';					\
		} else if( move_len > 0 ) {				\
			*str_len -= move_len;				\
			memmove( str, c, sizeof(C)*(*str_len + 1)); /* Copy the null character */ \
		}							\
									\
		return str;						\
	}

L_STRING(ADJUSTL);
L_WSTRING(ADJUSTL);      

#define adjustl(S) __##S##adjustl

#define TRIM(S,C)							\
	static C *__##S##trim( C *str, size_t *str_len )		\
	{								\
		C *c = str + (*str_len);				\
		while( c != str &&  *(c-1) == ' ' ) {			\
			--c;						\
		}							\
		*str_len -= str + (*str_len) - c;			\
		*c = '\0';						\
		return str;						\
	}

L_STRING(TRIM);
L_WSTRING(TRIM);

#define trim(S) __##S##trim

#define BDS_STRING_CONTAINS(N,S,C)					\
	bool bds_##N##_contains( const C *str, const C *substr )	\
	{								\
		return ( S##str(str, substr ) != NULL );		\
	}


F_STRING(BDS_STRING_CONTAINS);
F_WSTRING(BDS_STRING_CONTAINS);

#define BDS_STRING_NUM_CONTAINS(N,S,C)					\
	size_t bds_##N##_num_contains( const C *str, const C *substr )	\
	{								\
		size_t num     = 0;					\
		const size_t substr_len = S##len(substr);		\
									\
		const C *s = str;					\
		while ((s = S##str(s, substr))) {			\
			++num;						\
			s += substr_len;				\
		}							\
		return num;						\
	}

F_STRING(BDS_STRING_NUM_CONTAINS);
F_WSTRING(BDS_STRING_NUM_CONTAINS);


#define BDS_STRING_ADJUSTL(N,S,C)					\
	C *bds_##N##_adjustl( C *str )					\
	{								\
		size_t str_len = S##len(str);				\
		return adjustl(S)( str, &str_len );			\
	}						

F_STRING(BDS_STRING_ADJUSTL);
F_WSTRING(BDS_STRING_ADJUSTL);

#define BDS_STRING_ADJUSTR(N,S,C)					\
	C *bds_##N##_adjustr( C *str )					\
	{								\
		const size_t str_len = S##len(str);			\
		const C *const str_end = str + str_len;			\
		const C *c = str_end;					\
									\
		while( c != str &&  *(c-1) == ' ' ) --c;		\
									\
		const long long move_len = str_end - c;			\
		if( move_len == str_len ) {				\
			*str = '\0';					\
		} else if( move_len > 0 ) {				\
			memmove( str + sizeof(C)*move_len, str, sizeof(C)*(str_len - move_len)); \
			memset( str, ' ', sizeof(C)*move_len);		\
		}							\
									\
		return str;						\
	}

F_STRING(BDS_STRING_ADJUSTR);
F_WSTRING(BDS_STRING_ADJUSTR);

#define BDS_STRING_TRIM(N,S,C)				\
	C *bds_##N##_trim( C *str )			\
	{						\
		size_t str_len = S##len(str);		\
		return trim(S)( str, &str_len );	\
	}

F_STRING(BDS_STRING_TRIM);
F_WSTRING(BDS_STRING_TRIM);

#define BDS_STRING_ATRIM(N,S,C)						\
	C *bds_##N##_atrim( C *str )					\
	{								\
		size_t str_len = S##len( str );				\
		return trim(S)( adjustl(S)( str, &str_len ), &str_len ); \
	}

F_STRING(BDS_STRING_ATRIM);
F_WSTRING(BDS_STRING_ATRIM);

#define BDS_STRING_TOUPPER(N,C,TOUPPER)			\
	C *bds_##N##_toupper(C *str)			\
	{						\
		C *s = str;				\
		while( *s ) {				\
			*s = TOUPPER(*s);		\
			++s;				\
		}					\
		return str;				\
	}						

BDS_STRING_TOUPPER(string,char,toupper);
BDS_STRING_TOUPPER(wstring,wchar_t,towupper);

#define BDS_STRING_TOLOWER(N,C,TOLOWER)			\
	C *bds_##N##_tolower(C *str)			\
	{						\
		C *s = str;				\
		while( *s ) {				\
			*s = TOLOWER(*s);		\
			++s;				\
		}					\
		return str;				\
	}						

BDS_STRING_TOLOWER(string,char,tolower);
BDS_STRING_TOLOWER(wstring,wchar_t,towlower);


#define BDS_STRING_TOKENIZE(N,S,C,STRTOK)				\
	void bds_##N##_tokenize( C *str, const C *delim,  size_t *num_tok, C **(*tok) ) \
	{								\
		size_t alloc_tok = 0;					\
		*tok = NULL;						\
		*num_tok = 0;						\
									\
		C *__str = str;						\
		C *next = NULL;						\
									\
		while( ( str = STRTOK( __str, delim, &next ) ) != NULL ) { \
			/* New string */				\
			append_tok(S)( str, &alloc_tok, num_tok, tok );	\
			__str = NULL;					\
		}							\
	}

BDS_STRING_TOKENIZE(string,str,char,strtok_r);
BDS_STRING_TOKENIZE(wstring,wcs,wchar_t,wcstok);

#define BDS_STRING_TOKENIZE_W(N,S,C)					\
	void bds_##N##_tokenize_w( C *str, const C *delim, size_t *num_tok, C **(*tok) ) \
	{								\
		const C * const str_end = str + S##len(str);		\
		const size_t delim_len     = S##len(delim);		\
									\
		size_t alloc_tok = 0;					\
		*tok = NULL;						\
		*num_tok = 0;						\
									\
		C *__str = str;						\
		C *c;							\
									\
		while( ( c = S##chr( __str, delim[0] ) ) != NULL ) {	\
									\
			if( str_end - c < delim_len )			\
				break;					\
									\
			if( S##ncmp( c, delim, delim_len ) == 0 ) {	\
				/* Do not include empty strings	*/	\
				if( c - str > 0 ) {			\
					/* New string */		\
					*c = '\0';			\
					append_tok(S)( str, &alloc_tok, num_tok, tok ); \
				}					\
				__str = str = c + delim_len;		\
			} else {					\
				__str = c + 1;				\
			}						\
									\
			if( __str >= str_end )				\
				break;					\
		}							\
									\
		/* Take care of trailing string */			\
		if( *num_tok > 0 && str < str_end )			\
			append_tok(S)( str, &alloc_tok, num_tok, tok ); \
									\
	}

F_STRING(BDS_STRING_TOKENIZE_W);
F_WSTRING(BDS_STRING_TOKENIZE_W);

#define BDS_STRING_SUBSTR(N,S,C)				\
	C *bds_##N##_substr( const C *str, size_t len )		\
	{							\
		if( len > S##len(str) )				\
			return NULL;				\
								\
		C *substr = malloc( sizeof(C)*(len + 1) );		\
		memcpy( substr, str, sizeof(C)*len );			\
		substr[len] = '\0';					\
									\
		return substr;						\
	}

F_STRING(BDS_STRING_SUBSTR)
F_WSTRING(BDS_STRING_SUBSTR)

#define BDS_STRING_FIND(N,S,C)						\
	C *bds_##N##_find( const C *str, const C *seq )			\
	{								\
		return S##str( str, seq );				\
	}								\

F_STRING(BDS_STRING_FIND)
F_WSTRING(BDS_STRING_FIND)

#define BDS_STRING_RFIND(N,S,C)						\
	C *bds_##N##_rfind( const C *str, const C *seq )		\
	{								\
		const size_t seq_len = S##len(seq);			\
		const C *s        = str + S##len(str) - seq_len;	\
									\
		while( s >= str ) {					\
			while( *s != seq[0] ) {				\
				if( s == str ) break;			\
				--s;					\
			}						\
									\
			if( S##ncmp( s, seq, seq_len ) == 0 ) {		\
				return (C *)s;				\
			}						\
			--s;						\
		}							\
									\
		return NULL;						\
	}

F_STRING(BDS_STRING_RFIND);
F_WSTRING(BDS_STRING_RFIND);

#define BDS_STRING_ISNUM(N,S,C,ISDIGIT)				\
	bool bds_##N##_isnum(const C *str )			\
	{							\
		const C * const str_end = str + S##len(str);	\
		while( str != str_end ) {			\
			if( ! ISDIGIT(*str) )			\
				return false;			\
			++str;					\
		}						\
		return true;					\
}

BDS_STRING_ISNUM(string,str,char,isdigit);
BDS_STRING_ISNUM(wstring,wcs,wchar_t,iswdigit);


#define BDS_STRING_DUP(N,S,C)						\
	C *bds_##N##_dup(const C *str)					\
	{								\
		const size_t str_len = S##len(str);			\
		C *s = malloc(sizeof(C) * (str_len + 1));		\
		if (s) {						\
			memcpy(s, str, sizeof(C) * (str_len + 1));	\
		}							\
		return s;						\
	}

F_STRING(BDS_STRING_DUP);
F_WSTRING(BDS_STRING_DUP);

#define BDS_STRING_DUP_CONCAT(N,S,C)					\
	C *bds_##N##_dup_concat(int num_str, const C *str, ...) \
	{								\
		va_list ap;						\
									\
		size_t str_len_tbl[num_str];				\
		const C *str_tbl[num_str];				\
		size_t str_len = 0;					\
									\
		str_tbl[0]     = str;					\
		str_len_tbl[0] = S##len(str_tbl[0]);			\
		str_len        = str_len_tbl[0];			\
									\
		va_start(ap, str);					\
		for (int i = 1; i < num_str; ++i) {			\
			str_tbl[i]     = va_arg(ap, const C *);		\
			str_len_tbl[i] = S##len(str_tbl[i]);		\
			str_len += str_len_tbl[i];			\
		}							\
		va_end(ap);						\
									\
		C *str_dup = malloc(sizeof(C) * (str_len + 1));		\
									\
		if (!str_dup) {						\
			return NULL;					\
		}							\
									\
		C *s = str_dup;						\
		for (int i = 0; i < num_str; ++i) {			\
			/* Copy null terminator */			\
			memcpy(s, str_tbl[i], sizeof(C) * (str_len_tbl[i] + 1)); \
			/* Advance to the null terminator */		\
			s += str_len_tbl[i];				\
		}							\
		return str_dup;						\
	}

F_STRING(BDS_STRING_DUP_CONCAT);
F_WSTRING(BDS_STRING_DUP_CONCAT);

#define BDS_STRING_PREPENDF(N,S,C,VSPRINTF)				\
	C *bds_##N##_prependf(C *dest, size_t max_len, const C *fmt, ...) \
	{								\
		C buf[max_len];						\
		S##ncpy(buf, dest, max_len);				\
									\
		va_list va;						\
		va_start(va, fmt);					\
		VSPRINTF(dest, max_len, fmt, va);			\
		va_end(va);						\
									\
		size_t __offset  = S##len(dest);			\
		size_t __max_len = (max_len >= __offset ? max_len - __offset : 0); \
		size_t buf_len = S##len(buf);				\
									\
		buf_len = (  buf_len < __max_len ? buf_len : __max_len ); \
									\
		memcpy(dest + __offset, buf, sizeof(C)*buf_len);	\
		memset(dest + __offset + buf_len, 0, sizeof(C));	\
									\
		return dest;						\
	}

BDS_STRING_PREPENDF(string,str,char, vsnprintf);
BDS_STRING_PREPENDF(wstring,wcs,wchar_t, vswprintf);


#define BDS_STRING_CONCATF(N,S,C,VSPRINTF)				\
	C *bds_##N##_concatf(C *dest, size_t max_len, const C *fmt, ...) \
	{								\
		size_t __offset  = S##len(dest);			\
		size_t __max_len = (max_len >= __offset ? max_len - __offset : 0); \
									\
		va_list va;						\
		va_start(va, fmt);					\
		VSPRINTF(dest + __offset, __max_len, fmt, va);		\
		va_end(va);						\
									\
		return dest;						\
	}

BDS_STRING_CONCATF(string,str,char, vsnprintf);
BDS_STRING_CONCATF(wstring,wcs,wchar_t, vswprintf);

#define BDS_STRING_COPYF(N,C,VSPRINTF)					\
	C *bds_##N##_copyf(C *dest, size_t max_len, const C *fmt, ...)	\
	{								\
		va_list va;						\
		va_start(va, fmt);					\
		VSPRINTF(dest, max_len, fmt, va);			\
		va_end(va);						\
		return dest;						\
	}

BDS_STRING_COPYF(string,char,vsnprintf);
BDS_STRING_COPYF(wstring,wchar_t,vswprintf);
