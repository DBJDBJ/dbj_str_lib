#pragma once
#include "dbj.h"
#include "dbj_str_sll.h"
/*
synopsis: 

Given: "Abra ka dabra!" and "ka", return an array { "Abra ", " dabra!" }

Usage contract:

-- input text must be left/right trimmed of all whitespaces and spaces
-- if boundary is found as suffix/prefix it will be ignored
   example: "++one++two++three++" is treated as "one++two++three"

Function signature:

unsigned int dbj_str_to_array ( 
      int * count,      -- count of the rezulting array of strings
	  char ** rezult,   -- rezulting array of strings
	 char text_ [],             -- input text
	 char boundary_ []          -- what to cut out
	 ) ;

On error: 

 -- set the errno to appropriate ISO dictated, errnum constant
 -- return this errno constant

 On sucess:

 Return 0
 Set the rezult pointer to array of  strings, and set its count rezult

*/

/*
return array of indexes, one for each boundary found
this are locations marking the beginning of gaps
*/
static dbj_array  * dbj_str_gaps_locations(char text_[], char boundary_[])
{
	// logic: the max size of gaps is max half of the input
	// example: "a|b|c|d", 3 gaps for input of uneven size of 7
	// (int)(7/2)  yields 3
	// note: we always trim of the possible
	// prefix/suffix boundaries if found
	// example: (size 8) "|a|b|c|d" -- becomes --> (size 7)"a|b|c|d"
	// example: (size 7) "|a|b|cd"  -- becomes --> (size 6)"a|b|cd"
	// cast to int rounds down by simply cutting of the fractional part
	int marks_len = 1 + (int)(strlen(text_) / 2);
	int * marks = dbj_stack_alloc(int, marks_len );
		memset(marks, 0, marks_len);
	int gap_count = 0;

	const char *str = text_;
	const char *begin = &str[0];
	const char *target = boundary_;
	const int   target_len = strlen(target);
	const int   gap_len = strlen(boundary_);
	const int   presumed_gap_suffix_pos = target_len - gap_len;
	const char *result = str;
	size_t      gap_location = 0;
	while ((result = strstr(result, target)) != NULL) 
	{
		gap_location = result - begin;
		// gap location 0 is ignored
		// example: "++one++two++three" 
		// is threated as "one++two++three"
		if (gap_location < 1) goto continuation;
		// gap suffix is ignored
		// example: "one++two++three++" 
		// is threated as "one++two++three"
		if (gap_location == presumed_gap_suffix_pos ) goto continuation;

		marks[gap_count] = gap_location ;
		// increment marks walker
		++gap_count;
		continuation: 
		// Increment result, otherwise we'll find target at the same location
		++result;
	}
	dbj_array * p = 0;
	dbj_array_make (p, marks, gap_count, int);
	// we have made a structure that holds size of int
	// count of array of ints and void * to the array of ints
	return p;
}

// return 0 on success
unsigned int dbj_str_to_array(
	int  *  count,		//--count of the rezulting array of strings
	char ** rezult,		//--rezulting array of strings
	char    text_[],	//--input text
	char    boundary_[]	//--what to cut out
)
{
	// are the input strings both zero limited?
	if (NULL == strchr(text_, 0)) {	return errno = EINVAL; }
	if (NULL == strchr(boundary_, 0)) { return errno = EINVAL; }
	// boundary not found ?
	if (NULL == strstr(text_, boundary_)) { return errno = EINVAL; }
	// is boundary larger or equal v.s. the text_ ?
	if (strlen(boundary_) >= strlen(text_)) { return errno = EINVAL; ; }

	dbj_array * gaps_ = dbj_str_gaps_locations(text_, boundary_);
	// are there any gaps located?
	if (gaps_->count_ < 1) {
		// free and return null
		dbj_array_free(gaps_);
		return errno = EINVAL; 
	}

	*count = 0;
	(*rezult) 
		=  dbj_calloc( char *, gaps_->count_ );
	int * locations_ 
		= dbj_array_cast(gaps_, int);
	int j = 0, 
		from_pos = 0, 
		to_pos = 0 ;
	const int gap_size 
		= strlen(boundary_);
	int word_len = 0;

	for (j = 0; j < dbj_array_count( gaps_ ); ++j)
	{
		to_pos = locations_[j];
		word_len = to_pos - from_pos;
		if (word_len > 0)
		{ 
			(rezult)[j] = dbj_strndup((const char *)text_[from_pos], word_len);
			(*count) += 1;
		}
		from_pos = to_pos + gap_size;
	}
	return errno = 0 ;
}

static void dbj_str_to_array_release_rezult(int * rezult_size, char * rezult[] ) {
	for (int j = 0; j < *rezult_size; ++j) {
		if (rezult[j]) free(rezult[j]);
	}
	*rezult_size = 0;
}

void dbj_test_str_to_array() {
	int rezult_size = 0;
	char * rezult[] = {0};
	char * text_ = "one two one three one four";

	char * new_text_ = dbj_str_remove_all(text_, " ");

	int failure = dbj_str_to_array
	(& rezult_size, rezult, new_text_, "one");

	if (failure) {
		char * err_mesaage_ = strerror(failure);
	}
	else {
		// print the result
		for (int j = 0; j < rezult_size; ++j) {
			if (rezult[j]) 
				dbj_dump_charr_arr(strlen(rezult[j]), rezult[j]) ;
		}
	}

	free(new_text_);
	dbj_str_to_array_release_rezult(&rezult_size, rezult);

	bool dumsy = false;
}