/* NOT IN USE! */
#pragma once
#include <assert.h>
#include <stdlib.h>
#include <errno.h>

#ifndef dbj_malloc
#define dbj_malloc(type, count) malloc( count * sizeof(type))
// allocate and zero an array
#define dbj_calloc(type, count) calloc(count, sizeof(type))
#endif

///  DBJ MATRIX ----------------------------------------------

/*
	NOTE! The result is void **
		  to free use single free()
*/
#define dbj_matrix_alloc(type, rows, cols) \
    calloc(sizeof(type *) * rows + sizeof(type) * rows * cols, 1)

typedef dbj_matrix dbj_matrix;
// keep matrix data and its rows/cols count together
typedef struct dbj_matrix {
	int     rows_;
	int     cols_;
	void ** data_;
} dbj_matrix;

// free in a single free() call
static void ** dbj_matrix_data_allocate(unsigned int rows, unsigned int cols) {
	return dbj_matrix_alloc(void *, rows, cols);
}
static void	   dbj_matrix_free(dbj_matrix * p) {
	assert(p);
	if (p) {
		assert(p->data_);
		if (p->data_)	free(p->data_);
		free(p); p = 0;
	}
}
static dbj_matrix * dbj_matrix_make(void *source, unsigned int rows, unsigned int cols) {
	assert(source);
	dbj_matrix * p = dbj_malloc(dbj_matrix, 1);
	if (p) {
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
