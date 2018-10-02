/* NOT IN USE! */
#pragma once
#include <assert.h>
#include <stdlib.h>
#include <errno.h>

// MSVC complains if result of allocation is not casted
#ifndef dbj_malloc
#ifdef _MSC_VER
#define dbj_malloc(type, count) (type *)malloc( count * sizeof(type))
#define dbj_calloc(type, count) (type *)calloc( count, sizeof(type))
#else
#define dbj_malloc(type, count) malloc( count * sizeof(type))
#define dbj_calloc(type, count) calloc( count, sizeof(type))
#endif
#endif

#ifndef size_t
typedef unsigned int size_t;
#endif
#ifndef uchar_t
typedef unsigned char uchar_t;
#endif

///  DBJ MATRIX ----------------------------------------------

/*
	Pleae be sure to read and understand the
	"Effective type" concept
	https://en.cppreference.com/w/c/language/object#Effective_type

	Usage (MSVC requires the cast) :
	int ** a = (int **)dbj_matrix_alloc(int, 5, 5);
	a[4][3] = 42;
	free(a);
*/
#define dbj_matrix_data_size(type, rows, cols) \
   (sizeof(type *) * rows + sizeof(type) * rows * cols)

#define dbj_matrix_alloc(type, rows, cols) \
    calloc(dbj_matrix_data_size(type, rows, cols), 1)

/* keep matrix data and its rows/cols count together */
typedef dbj_matrix dbj_matrix;
typedef struct dbj_matrix {
	size_t     rows_;
	size_t     cols_;
/*
Except for bit fields, objects are composed of contiguous sequences 
of one or more bytes, each consisting of CHAR_BIT bits, and can be 
copied with memcpy into an object of type unsigned char[n], where 
n is the size of the object. The contents of the resulting array 
are known as object representation.
*/
	size_t     data_size_;
	uchar_t *  data_;
} dbj_matrix;

static void	   dbj_matrix_free(dbj_matrix * p) {
	assert(p && p->data_);
	if (p) {
		if (p->data_)	free(p->data_);
		free(p); p = 0;
	}
}

#define dbj_matrix_make( TYPE_ , /* dbj_matrix * */ p, /* size_t */ rows, /* size_t */ cols)  \
do { \
	p = (dbj_matrix *)dbj_malloc(dbj_matrix, 1); \
	if (p) { \
		p->rows_ = rows; \
		p->cols_ = cols; \
		p->data_size_ = dbj_matrix_data_size(TYPE_, rows, cols); \
		p->data_ = (uchar_t *)calloc(p->data_size_, 1); \
		if (!p->data_) { errno = ENOMEM; } \
	} \
	else { \
		errno = ENOMEM;  \
} while (0);

#define dbj_matrix_cast(p_, type_) (type_**)p_->data_
#define dbj_matrix_rows(p_) p_->rows_
#define dbj_matrix_cols(p_) p_->cols_
#define dbj_matrix_slot(type,p_,r_,c_) ((type)p_->data_[r_][c_])
