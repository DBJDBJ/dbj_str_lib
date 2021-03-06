#include "pch.h"

#pragma warning( push)

#pragma warning( disable : 4018 )
#pragma warning( disable : 4200 )
#pragma warning( disable : 4267 )
#pragma warning( disable : 4244 )
#define _SDS_TEST_
#include "../dbj_sds/sds.h"

#pragma warning( pop )

/* no alloca found */
#define DBJ_LINE "---------------------------------------------------------------------"

#ifndef FREE
#define FREE(p) do{  if (p) free((void *)p); p = 0;}while(0)
#endif // !FREE


char * dbj_strncpy(const char * src, const size_t n)
{
	DBJ_ASSERT(sizeof(char) == 1);
	char * dst = (char *)calloc(n + 1, 1);
	DBJ_ASSERT(dst);
	/* few ticks slower but much safer */
	memmove(dst, src, n);
	/* dst is already initialized with all 0
	   that is the calloc behaviour
	   so there is no need for dst[n] = '\0' */
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

char * dbj_remove_first_substring(const char str[], const char sub[])
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
		printf("\n %3.3d  '%s'", (int)count_++, (dst ? dst : "NULL"));
		free((void *)dst);
	}
	printf("\n\n");
}

void test_dbj_rmv_sub(const char * balvan, const char * koren)
{
	printf("\n\n Dr DBJ-ovo Vadjenje korenja\n%s\n", DBJ_LINE);
	char * dst = 0;
#define SHOW(x) printf("\n %s\n Rezultat --> [%s]\n", #x, (x))
	SHOW(dst = dbj_remove_first_substring("++Imamo++kvalitetno++drvo++za++sve++meteo++uslove.", "++Imamo"));
	FREE(dst);
	SHOW(dst = dbj_remove_first_substring("Bilo kakvo stablo", ""));
	FREE(dst);
	SHOW(dst = dbj_remove_first_substring("Bilo kakvo stablo", "stablo"));
	FREE(dst);
#undef SHOW
	printf("\n");
}
/*
***********************************************************************
*/
int main(int argc, char *argv[])
{
#ifdef _SDS_TEST_
	sdsTest(argc, argv);
#endif
	dbj_string_test();
	static  const char * data_str = "++Imamo++kvalitetno++drvo++za++sve++meteo++uslove.";
	static  const char * data_cut = "++";
	test_next_cut(data_str, data_cut);
	test_dbj_rmv_sub(data_str, data_cut);
	return errno;
}

