#pragma once
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
/*
	DBJ ARRAY is handled with structure and functions/macros
	operating on them
*/
// keep array data and its length together
typedef struct dbj_array_struct {
	int    slot_size_; // size of type kept in the array 
	int    count_;
	void * data_;  // keep the data attached
} dbj_array;

static void * dbj_array_data_alloc(unsigned int count_) {
	return dbj_calloc(void *, count_);
}
static void  dbj_array_free(dbj_array * p) {
	assert(p);
	if (p) {
		assert(p->data_);
		if (p->data_) free(p->data_);
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
