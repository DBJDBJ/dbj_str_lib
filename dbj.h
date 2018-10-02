#pragma once
// common stuff
// on wandbox compiled with this cl: $gcc prog.c
// no C99 no C11
// gcc version 4.4.7

#ifdef _MSC_VER
#include "windows.h"
#endif

#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

// only static extern variables can use thread local storage
#ifdef _MSC_VER
#define dbj_thread_local __declspec(thread) static
#else
#define dbj_thread_local __thread static
#endif

// in C do not cast the result of malloc/calloc etc
// that will hide important warnings!
#define dbj_malloc(type, count) malloc( count * sizeof(type))

// stack allocation, no free necessary, but use sparringly!
#ifndef _MSC_VER
#define  dbj_stack_alloc(type, count) (type*)alloca( count * sizeof(type))
#else
#define dbj_stack_alloc(type, count) _malloca( count * sizeof(type))
#endif

// allocate and zero an array
#define dbj_calloc(type, count) calloc(count, sizeof(type))
/*
advanced DBJ lesson :)
	int *p1 = dbj_calloc(4, int);    // allocate and zero out an array of 4 int
	int *p2 = dbj_calloc(1, int[4]); // same, naming the array type directly
	int *p3 = dbj_calloc(4, *p3);    // same, without repeating the type name
*/

/*
strdup and strndup are defined in POSIX compliant systems as :

char *strdup(const char *str);
char *strndup(const char *str, size_t len);
*/
static char * dbj_strdup(const char *s) 
{
	char *d = (char *)dbj_malloc(char *, strlen(s) + 1);   // Space for length plus nul
	if (d == NULL) { errno = ENOMEM; return NULL; }// No memory
	strcpy(d, s);                        // Copy the characters
	return d;                            // Return the new string
}

/*
The strndup() function copies at most len characters from the string str
always null terminating the copied string.
input does not have to be zero terminated
*/
static char * dbj_strndup(const char str_[], const size_t n)
{
	assert(str_);
	char *result = (char *)dbj_calloc(char *, n + 1);
	if (result == NULL) { errno = ENOMEM; return NULL; }  // No memory
		memmove( result, str_, n );
			result[n] = '\0';
	return result;
}

/*
return the index of the substring found or -1
*/
static int dbj_substr_pos( const char str_ [], const char sub_ []) {
	char* pos = strstr(str_, sub_);
	return (pos != NULL ? pos - str_ : -1 );
}

// is boundary prefix to the input?
static bool is_dbj_str_prefix(const char str_[], const char pfx_[]) {
	return (0 == dbj_substr_pos(str_, pfx_));
}
// is boundary prefix to the input?
static bool is_dbj_str_suffix(const char str_[], const char sfx_[]) 
{
	assert(str_);
	assert(sfx_);

	size_t str_len_ = strlen(str_) ;
	size_t pfx_len_ = strlen(sfx_) ;

	assert(str_len_);
	assert(pfx_len_);

	size_t presumed_suffix_location = str_len_ - pfx_len_;
	char * presumed_suffix = (char*)&str_[presumed_suffix_location];
	if (0 == dbj_substr_pos(presumed_suffix, sfx_))
	{
		// boundary found to be suffix
		return true;
	}
	return false;
}

static void dbj_dump_charr_arr(size_t size_, char *str, bool also_binary )
{
	char *p = str;
	if (also_binary) {
		// print the codes first
		printf("[");
		for (size_t n = 0; n < size_; ++n) { printf("%2.2x ", *p);	++p; }
		printf("], ");
	}
    // print the chars second
	printf("\"");
	p = str;
	for (size_t n = 0; n < size_ ; ++n)	{ printf("%c", *p ? *p : ' ');	++p; }
	printf("\"");

	// printf("\n", str);
}

// the rest ///////////////////////////////////////////////////////////

extern char * dbj_str_remove_all(const char * str_, const char * chars_to_remove_);
extern void dbj_test_str_remove();

extern char * dbj_str_remove_from_to(char str[], int slice_from, int slice_to);
extern void test_dbj_str_remove_from_to();
/*
return 0 on OK, errno on error
after new_front and new_back are moved if prefix/sufix is found
*/
extern int dbj_str_remove_prefix_suffix(
	char ** new_front,
	char ** new_back,
	const char text_[],
	const char boundary_[]
);

///  EOF
