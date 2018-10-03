#pragma once

#include "dbj_sawmill.h"

#if defined (_MSC_VER)
#define PLATFORM "Windows"
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#elif defined (__linux)
#define PLATFORM "Linux"
#define dbj_strerror strerror
#endif
/*
Note: while inside c++ all is in the dbj::clib namespace
*/
#ifdef __cplusplus
extern "C" {
#endif

	char * dbj_strerror(errno_t errno_) {
#ifdef _MSC_VER
		static char buffer[1024] ;
		memset(buffer, 0, 1024);
		strerror_s( buffer, 1024, errno_);
		return buffer;
#else
		return strerror(errno_);
#endif
	}
/* return 0 or errno */
static int dbj_chararr_release(size_t count, char * rezult[]) {

	if (rezult == NULL) { return errno = EINVAL;  }

		size_t n; /* C98 zahteva ovo ovde a ne u for() */
		for (n = 0; n < count; ++n) {
			if (rezult[n]) {
				free(rezult[n]); rezult[n] = 0;
			}
		}
		free(rezult);
		rezult = 0;
	return 0;
}

/*
from dbj sll make char * [] and return it
with the count
*/
static int dbj_sll_to_chararr(char ** rezult)
{
	static const int BAD_RET_VAL = 0;
	int retval = BAD_RET_VAL;
		if (rezult != NULL) {
			errno = EINVAL; return BAD_RET_VAL;
		}
		/*
		rezult = (char **)malloc(sizeof(char **));
		*/
	return retval;
}

static void chararr_print(const size_t rezult_size, const char * rezult[]) {
	printf("\n\n " PRODUCT_ID ", %3.3d rows\n", (int)rezult_size);
	size_t j;
	for (j = 0; j < rezult_size; ++j) {
		if (rezult[j]) {
			printf("\n%3.3d %s", (int)j, rezult[j]);
		}
	}
}

#ifdef DBJ_SAWMILL_TESTING

static void unit_test_dbj_sawmill(
	const char text_[], 
	const char boundary_[]
)
{
	/*
	dbj_sawmill leaves the result in the DBJ SLL instance
	*/
	dbj_sll_node * sll_ = dbj_sll_make_head();
	assert(sll_);
	int failure = dbj_sawmill(sll_, text_, boundary_);
	if (failure) {
		printf("\n\n" PRODUCT_ID " error message:%s\n", dbj_strerror(errno));
	}
	else {
		int how_many_nodes = dbj_sll_count(sll_);
		printf("\n\nInput:[%s], boundary:[%s]\n", text_, boundary_ );
		printf("\n" PRODUCT_ID " result dump\t%3.3d elements created\n", how_many_nodes );
		dbj_sll_foreach(sll_, dbj_sll_node_dump_visitor);
		printf("\n");
		/* remove the nodes, remove the head */
		dbj_sll_erase_with_head(sll_);
	}
	system("pause");
	return;
}

extern const char large_text[];

static void test_dbj_sawmill() {
	const  char * text_ = "one two one three one four one", *boundary = "one";
	unit_test_dbj_sawmill(text_, "one");
	unit_test_dbj_sawmill(text_, "two");
	unit_test_dbj_sawmill(text_, "wrong");
	unit_test_dbj_sawmill(text_, "");
}

static const char large_text[] = {
"Spaces have been removed from the input str before calling."
"Thus the input :"
"str == 'onetwoonethreeonefour'"
"brk == 'one'"
"The desired output is the list : {'two', 'three', 'four'}"
"Algorithm Description"
"Core of the processing is the(internal) function next_element()"
"char * next_element(char ** /*str*/, char * /*brk*/);"
"Return is next element obtained"
"Processing stops when the first argument points to NULL."
"Given the input as above next_element(), behaves like this"
"(without all arg's repeated etc...):"
"	char * next_element('onetwoonethreeonefour')-- > str == 'twoonethreeonefour', returns NULL"
"	char * next_element('twoonethreeonefour')-- > str == 'threeonefour', returns 'two'"
"	char * next_element('threeonefour')-- > str == 'four', returns 'three'"
"	char * next_element('four')-- > str == 0, returns 'four'"
"	Processing stops"
};
#endif  /* DBJ_SAWMILL_TESTING */

#ifdef __cplusplus
	} // extern "C"
#endif

