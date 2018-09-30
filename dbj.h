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
	int *p1 = dbj_array_alloc(4, int);    // allocate and zero out an array of 4 int
	int *p2 = dbj_array_alloc(1, int[4]); // same, naming the array type directly
	int *p3 = dbj_array_alloc(4, *p3);    // same, without repeating the type name
*/

/* 
    NOTE! The result is void ** 
	      to free use single free() 
*/
#define dbj_matrix_alloc(type, rows, cols) \
    calloc(sizeof(type *) * rows + sizeof(type) * rows * cols, 1)
/*
Usage :
int ** a = dbj_matrix_alloc(int, 5, 5);
a[4][3] = 42;
*/
/*
    DBJ ARRAY & MATRIX are handled with two structures and functions/macros
	operating on them
*/
// keep array data and its length together
typedef struct dbj_array_struct {
	int    slot_size_; // size of type kept in the array 
	int    count_;
	void * data_;  // keep the data attached
} dbj_array ;

static void * dbj_array_data_alloc( unsigned int count_) {	
	return dbj_calloc(void *, count_);
}
static void  dbj_array_free (dbj_array * p) { 
	assert(p);
	if (p) {
		assert(p->data_);
		if(p->data_) free(p->data_); 
		free(p); p = 0;
	}
}
// static dbj_array * dbj_array_make(void *source, int count, size_t value_size) {
// p is dbj_array *
#define dbj_array_make( p, source, count, type ) \
do {\
	assert(source);\
    p = dbj_malloc(dbj_array, 1);\
	if (p){\
		p->slot_size_ = sizeof(type) ;\
		p->count_ = count;\
		type * source_copy  = dbj_calloc( type, count );\
        for (int n = 0; n < count; ++n) { source_copy[n] = source[n]; }\
		p->data_ = source_copy;\
	}\
	else {\
		errno = ENOMEM; \
	}\
} while(0)

#define dbj_array_cast(p_, type_) (type_*)p_->data_
#define dbj_array_count(p_) p_->count_
// get to the pointer to the value in the array
// thus we can change it, not just use it
#define dbj_array_val_ptr(p_, type, pos, rezult) \
do { \
assert(p_ && p_->data_); \
rezult = (type*)p_->data_ ; \
if ( (pos > 0) && (pos < p_->count_) ) { rezult = (type *)(rezult + pos) ; } \
else { rezult = (type *)0; } \
} while(0)
/* 
point to void * can not be plus incremented 
above is horrible and necessary in C since we can not pass the type
to the functions in C, the macro is the only way
*/

///  DBJ MATRIX ----------------------------------------------
// keep matrix data and its rows/cols count together
typedef struct dbj_matrix_struct {
	int     rows_;
	int     cols_;
	void ** data_;
} dbj_matrix ;

// free in a single free() call
static void ** dbj_matrix_data_allocate( unsigned int rows, unsigned int cols ) {
	return dbj_matrix_alloc(void *, rows, cols);
}
static void	   dbj_matrix_free (dbj_matrix * p) { 
	assert(p);
	if (p) {
		assert(p->data_);
		if ( p->data_)	free(p->data_); 
		  free(p); p = 0;
	}
}
static dbj_matrix * dbj_matrix_make(void *source, unsigned int rows, unsigned int cols) {
	assert(source);
	dbj_matrix * p = dbj_malloc(dbj_matrix, 1);
	if (p){
		p->rows_ = rows;
		p->cols_ = cols;
		p->data_ = dbj_matrix_data_allocate(rows, cols);
		if (!p->data_) return NULL; // errno == ENOMEM
	}
	else {
		errno = ENOMEM; return NULL; // no memory
	}
	return p;
}
#define dbj_matrix_cast(p_, type_) (type_**)p_->data_
#define dbj_matrix_rows(p_) p_->rows_
#define dbj_matrix_cols(p_) p_->cols_

/*
strdup and strndup are defined in POSIX compliant systems as :

char *strdup(const char *str);
char *strndup(const char *str, size_t len);
*/
static char * dbj_strdup(const char *s) 
{
	char *d = dbj_malloc(char *, strlen(s) + 1);   // Space for length plus nul
	if (d == NULL) { errno = ENOMEM; return NULL; }// No memory
	strcpy(d, s);                        // Copy the characters
	return d;                            // Return the new string
}

/*
The strndup() function copies at most len characters from the string str
always null terminating the copied string.
*/
static char * dbj_strndup(const char *s, size_t n)
{
	char *result = 0;
	size_t len = strlen(s);

	if (n < len)
		len = n;

	result = dbj_malloc(char *, len + 1);
	if (result == NULL) {errno = ENOMEM;return NULL;}  // No memory

	result[len] = '\0';
	return (char *)memcpy(result, s, len);
}

void dump_charr_arr(size_t size_, char *str)
{
	// print the codes first
	char *p = str;
	for (size_t n = 0; n < size_ ; ++n)	{ printf("%2.2x ", *p);	++p;}
	printf("\t");
    // print the chars second
	p = str;
	for (size_t n = 0; n < size_ ; ++n)	{ printf("%c", *p ? *p : ' ');	++p; }

	// printf("\n", str);
}

// the rest 

#include "dbj_str_remove.h"
#include "dbj_str_remove_from_to.h"
#include "dbj_str_to_array.h"
