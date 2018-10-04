
#include "pch.h"

/*
Also at
https://wandbox.org/permlink/k3J1ZYeRtTIMRgd1
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#define DBJ_ASSERT _ASSERTE
#else
#include <assert.h>
#define DBJ_ASSERT assert
#endif

#ifdef __cplusplus
extern "C" {
#endif
	/* no alloca found */
#define DBJ_LINE "---------------------------------------------------------------------"

#define FREE(p) do{  if (p) free((void *)p); p = 0;}while(0)

	char * dbj_strncpy(const char src[], const size_t n)
	{
		DBJ_ASSERT(sizeof(char) == 1);
		char * dst = (char *)calloc(n + 1, 1);
		DBJ_ASSERT(dst);
		/* few ticks slower but much safer */
		memmove(dst, src, n);
		/* zero limit the output */
		dst[n] = '\0';
		return dst;
	}

#define DBJ_STRDUP(s) dbj_strncpy(s,strlen(s))

	char * next_cut(char ** string, const char breaker[])
	{
		/* only the breaker is left */
		if (0 == strcmp(*string, breaker))
		{
			*string = 0;
			return 0;
		}

		char * p = (char *)strstr(*string, breaker);
		size_t size_left_of_breaker = p - *string;
		size_t brklen = strlen(breaker);

		/* no breaker found */
		if (!p)
		{
			char * retval = DBJ_STRDUP(*string);
			*string = 0;
			return retval;
		}

		/* breaker is the prefix */
		if (size_left_of_breaker == 0) {
			*string += brklen;
			/* goto next cut */
			return next_cut(string, breaker);
		}

		/* take the front as the cut */
		char * retval = dbj_strncpy(*string, size_left_of_breaker);
		/*  move the front pointer behind the breaker */
		*string += (size_left_of_breaker + brklen);
		/***/
		return retval;
	}

	char * dbj_rmv_sub_(const char str[], const char sub[])
	{
		DBJ_ASSERT(str && sub);
		char * p = (char *)strstr(str, sub);
		if (p)
		{
			char * dst = DBJ_STRDUP(str);
			p = (char *)strstr(dst, sub);
			strcpy(p, p + strlen(sub));
			return dst;
		}
		errno = EINVAL;
		return NULL;
	}



	void test_next_cut(const char * balvan, const char * cut_point)
	{
		char * dst = 0,
			*walker = (char *)balvan;

		printf("\n\n Dr DBJ-ova Pilana\n%s", DBJ_LINE);
		printf("\n\n Balvan:'%s'\n\n Ovde seci:\t'%s'\n", balvan, cut_point);
		size_t count_ = 0;
		while (walker) {
			dst = next_cut(&walker, cut_point);
			printf("\n %3.3d  '%s'", count_++, (dst ? dst : "NULL"));
			free((void *)dst);
		}
		printf("\n\n");
	}

	void test_dbj_rmv_sub(const char * balvan, const char * koren)
	{
		printf("\n\n Dr DBJ-ovo Vadjenje korenja\n%s\n", DBJ_LINE);
		char * dst = 0;
#define SHOW(x) printf("\n %s\n Rezultat --> [%s]\n", #x, (x))
		SHOW(dst = dbj_rmv_sub_("++Imamo++kvalitetno++drvo++za++sve++meteo++uslove.", "++Imamo"));
		FREE(dst);
		SHOW(dst = dbj_rmv_sub_("Bilo kakvo stablo", ""));
		FREE(dst);
		SHOW(dst = dbj_rmv_sub_("Bilo kakvo stablo", "stablo"));
		FREE(dst);
#undef SHOW
		printf("\n");
	}
	/*
	***********************************************************************
	*/
	int main()
	{
		static  const char * data_str = "++Imamo++kvalitetno++drvo++za++sve++meteo++uslove.";
		static  const char * data_cut = "++";
		test_next_cut(data_str, data_cut);
		test_dbj_rmv_sub(data_str, data_cut);
		return errno;
	}

#ifdef __cplusplus
} /* extern "C" { */
#endif
