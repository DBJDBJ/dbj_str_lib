
#include "pch.h"
#include <string_view>

#ifdef _MSC_VER
#include <crtdbg.h>
#define DBJ_ASSERT _ASSERTE
#else
#include <assert.h>
#define DBJ_ASSERT assert
#endif
/*

Also on WANDBOX
---------------
https://wandbox.org/permlink/JUPX47qjFr1HWmjM

*/
#define DBJ_SAWMILL_TESTING
#include "../dbj_testera/dbj_sawmill_util.h"

#ifdef __cplusplus
extern "C" {
#endif
	char * dbj_strncpy(const char src[], const size_t n) 
	{
		DBJ_ASSERT(sizeof(char) == 1);
		char * dst = (char *)calloc(n + 1, sizeof(char) );
		DBJ_ASSERT(dst);
		memmove(dst, src, n);
		// zero limit
		dst[n] = '\0';
		return dst;
	}

	char * next_cut ( char ** string , const char breaker[])
	{
		// only breaker left
		if (0 == strcmp(*string, breaker))
		{
			*string = 0;
			return 0;
		}

		char * p = (char *)strstr(*string, breaker);
		size_t size_left_of_breaker = p - *string;
		size_t brklen = strlen(breaker);

		// no breaker found
		if ( ! p)
		{
			char * retval = _strdup(*string);
			*string = 0;
			return retval;
		}
		
		// breaker is the prefix
		if (size_left_of_breaker == 0) {
			*string += brklen;
			// goto next cut
			return next_cut( string, breaker);
		}

		// take the front as the cut
			char * retval = dbj_strncpy(*string, size_left_of_breaker);
        //  move the front pointer behind the breaker
			*string += (size_left_of_breaker + brklen);
			//
			return retval;
	}

	char * dbj_rmv_sub_(const char str[], const char sub[])
	{
		DBJ_ASSERT(str && sub);
		char * p = (char *)strstr(str, sub);
		if (p)
		{
			char * dst = _strdup(str);
			p = (char *)strstr(dst, sub);
			strcpy(p, p + strlen(sub));
			return dst;
#if 0
			/*	this would be easy solution: strcpy(p, p + strlen(sub));
				but above hack does SEGV on read only strings,	so ...
				*/
			const size_t  sublen = strlen(sub);					DBJ_ASSERT(sublen);
			const size_t  slen = strlen(str);					DBJ_ASSERT(slen);
			const size_t  newlen = (slen - sublen);				DBJ_ASSERT(newlen);
			char		  *last = strchr((char *)str, 0);		DBJ_ASSERT(last);
			/* allocate the result buffer */
			char		  * dst = (char *)calloc( newlen, 1 );	DBJ_ASSERT(dst);
			/* copy from the left of the sub */
			memmove(dst, str, p - str);
			/* copy from the right of the sub */
			memmove(dst + (p - str), p + sublen, (last - p) + sublen);
			/* make sure dst iz zero limited */
			/* if this is "one beyond" free will SEGV */
			dst[newlen] = '\0';
			return dst;
#endif
		}
		errno = EINVAL;
		return NULL;
	}

#include <stdarg.h>
	void free_free_set_them_free(void * vp, ...)
	{
		static const unsigned int max_arg_count = 255;
		unsigned int cicrcuit_breaker = 0;
		va_list marker;
		va_start(marker, vp); /* Initialize variable arguments. */
		while (vp != NULL)
		{
			free(vp);
				if (cicrcuit_breaker++ > max_arg_count) break;
			vp = va_arg(marker, void *);

		}
		va_end(marker);   /* Reset variable argument list. */
	}
#ifndef _MSC_VER
// gcc has no problem with this
#define FREE(...) free_free_set_them_free((void *)__VA_ARGS__)
#else
#define FREE(p) do{  if (p) free((void *)p); p = 0;}while(0)
#endif

// if you have alloca
#define DBJ_LINE(L,C)(char*)memset(memset(alloca(L), '\0', L), C, L-1)

	typedef struct test_data {
		static	const char * str;
		static	const char * cut;
	} test_data;
const char * test_data::str = "++Imamo++kvalitetno++drvo++za++sve++meteo++uslove.";
const char * test_data::cut = "+";

void test_next_cut( const char * balvan, const char * cut_point ) 
{
	char * dst = 0, 
		*walker = (char *)balvan; // &char_arr_str_[0];

	printf("\n\n Dr DBJ-ova Pilana\n%s", DBJ_LINE(60,'-'));
	printf("\n\n Balvan:'%s'\n\n Ovde seci:\t'%s'\n", balvan, cut_point);
	size_t count_ = 0;
	while (walker) {
		dst = next_cut(&walker, cut_point);
		printf("\n %3.3d  '%s'", count_++, (dst ? dst: "NULL"));
		free((void *)dst);
	} 
	printf("\n\n");
}

void test_dbj_rmv_sub(const char * balvan, const char * koren)
{
	printf("\n\n Dr DBJ vadjenje korenja\n");
	printf("\n Balvan: '%s', koren: '%s'", balvan, koren);
	char * dst = 0;
#define SHOW(x) printf("\n %s\n Rezultat --> [%s]\n", #x, (x))
	SHOW(dst = dbj_rmv_sub_(balvan, koren));
	FREE(dst);
	SHOW(dst = dbj_rmv_sub_("Bilo kakvo stablo", ""));
	FREE(dst);
	SHOW(dst = dbj_rmv_sub_("Bilo kakvo stablo","stablo"));
	FREE(dst);
#undef SHOW
	printf("\n");
}
/*
***********************************************************************
*/
int main()
{
	// test_dbj_rmv_sub_();
	test_next_cut(test_data::str, test_data::cut);
	test_dbj_rmv_sub(test_data::str, test_data::cut);
}

#ifdef __cplusplus
} /* extern "C" { */
#endif
