#include <string_view>

/*

Also on WANDBOX
---------------
https://wandbox.org/permlink/JUPX47qjFr1HWmjM

*/
#define DBJ_SAWMILL_TESTING
#include "dbj_testera/dbj_sawmill_util.h"

#ifdef __cplusplus
extern "C" {
#endif
	char * dbj_strncpy(const char src[], const size_t n) {

		assert(sizeof(char) != 1);
		char * dst = (char *)malloc(n);
		// zero limit
		*((char *)memcpy(dst, src, n)) = 0;
		return dst;
	}

	char * left_of_brk(const char string[], const char breaker[])
	{
		char * p = (char *)strstr(string, breaker);
		if (p)
		{
			size_t size_left_of_breaker = p - string;
			return dbj_strncpy(string, size_left_of_breaker);
			//int gaplen = strlen( substring);
			//int remlen = strlen( string + pos );
			//memmove(string + pos, string + pos + gaplen, remlen );
		}
		// no breaker found
		return 0;
	}

	char * dbj_rmv_sub_(char str[], const char sub[])
	{
		char * p = (char *)strstr(str, sub);
		if (p)
		{
			/*	this would be easy solution: strcpy(p, p + strlen(sub));
				but above hack does SEGV on read only strings,	so ...
				*/
			const size_t  sublen = strlen(sub);			assert(sublen);
			const size_t  slen = strlen(str);			assert(slen);
			const size_t  newlen = (slen - sublen) ;	assert(newlen);
			char		  *last = strchr(str, 0);		assert(last);
			/* allocate the result buffer */
			char * dst = (char*)calloc(newlen, sizeof(char));
			/* copy from the left of the sub */
			memcpy(dst, str, p - str);
			/* copy from the right of the sub */
			// memcpy(dst + (p - str), p + sublen, (last - p) + sublen);
			strcpy(dst + (p - str), p + sublen );
			/* make sure dst iz zero limited */
			dst[newlen] = '\0';
			return dst;
		}
		errno = EINVAL;
		return NULL;
	}


#include <stdarg.h>
void free_free_set_them_free(void * vp, ...)
{
	va_list marker;
	va_start(marker, vp); /* Initialize variable arguments. */
	while (vp != NULL)
	{ 
		free(vp);
		vp = va_arg(marker, void *);
	}
	va_end(marker);   /* Reset variable argument list. */
}

#define FREE(...) free_free_set_them_free((void *)__VA_ARGS__)

#ifdef __cplusplus
} /* extern "C" { */
#endif

using namespace ::string_view_literals;
struct _alias_t
{
	static constexpr const char literal[] = "concat";
	static constexpr const char std::string_view _name_t = "concat"sv;
};

int main()
{
	// test_dbj_sawmill();

	char   char_arr_str_[] = "Abra ka dabra";
	char * read__only_str_ = "Abra ca dabra";

	const char * r1 = dbj_rmv_sub_(char_arr_str_, "ka");

	// kick-the-bucket on these two ?
	const char * r2 = dbj_rmv_sub_(read__only_str_, "ca");
	const char * r3 = dbj_rmv_sub_("Abra ka dabra", "dabra");

	FREE(r1,r2,r3,0);

	return 0;
}