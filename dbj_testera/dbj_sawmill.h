#pragma once
/*
on wandbox compiled with this cl: $gcc prog.c
no C99 no C11
gcc version 4.4.7
*/
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#if defined (_MSC_VER)
#define PLATFORM "Windows"
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#elif defined (__linux)
#define PLATFORM "Linux"
#endif

/*
Note: while inside c++ all is in the dbj::clib namespace
*/
#ifdef __cplusplus
namespace dbj {
	namespace clib {
		extern "C" {
#endif

#pragma region dbj micro lib
/* only *static extern* variables can use thread local storage */
#ifdef _MSC_VER
#define dbj_thread_local __declspec(thread) static
#define dbj_malloc(type, count) (type *)malloc( count * sizeof(type))
#else
#define dbj_thread_local __thread static
#define dbj_malloc(type, count) malloc( count * sizeof(type))
#endif
#ifndef size_t
typedef unsigned int size_t;
#endif
#ifndef uchar_t
typedef unsigned char uchar_t;
#endif

extern char * dbj_strdup(const char *s);

#pragma endregion dbj micro lib

/*
   DBJ Singly Linked List aka DBJ SLL
   Also defines and holds the Thread Global Single Head  
   reachable through:

   static dbj_sll_node * dbj_sll_tls_head();

   For the simple API please see the header
*/
#include "dbj_sll.h"

#define PRODUCT_ID "DBJ Testera(tm)"
/*
	DBJ Sawmill(tm)
Synopsis:

Given: "Abra ka dabra!" and "ka", return an array { "Abra ", " dabra!" }

Usage contract:

-- input text must be left/right trimmed of all whitespaces and spaces
-- if boundary is found as suffix/prefix it will be ignored
   example: "++one++two++three++" is treated as "one++two++three"

Function signature:

int dbj_sawmill (
	 char text_ [],             -- input text
	 char boundary_ []          -- what to cut out
	 ) ;

On error:	Return the errno

 -- set the errno to appropriate ISO dictated, errnum constant
 -- return this errno constant

 On sucess: Return 0

 Rezult:

 Is in the nodes of the DBJ Thread Global SLL
 
*/
int dbj_sawmill(
	dbj_sll_node * sll_ ,		/* the sll head */
	const char    text_[],		/* what to cut  */
	const char    boundary_[]	/* where to cut */
);

#define dbj_pilana dbj_sawmill

#ifdef __cplusplus
	} // extern "C"
} // eof namespace clib 
} // eof namespace dbj 
#endif

