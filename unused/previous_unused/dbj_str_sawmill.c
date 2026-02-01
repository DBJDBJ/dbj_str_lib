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

unsigned int dbj_str_sawmill (
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

static void rekurzor(
	dbj_sll_node * sll_head,
	const char text_[], 
	const char boundary_[],
	const size_t gap_size
) 
{
	if (!text_) return;
	int gap =  dbj_substr_pos(text_, boundary_);
	if (-1 == gap) {
		// the last chunk
		dbj_sll_append(sll_head, text_);
		// chunk = dbj_strdup(text_);
		return;
	}
		char * chunk = dbj_strndup(text_ , gap );
		dbj_sll_append(sll_head,chunk);
		free(chunk);
		rekurzor(sll_head, &(text_)[gap + gap_size], boundary_, gap_size);
}

/*
return array of indexes, one for each boundary found
this are locations marking the beginning of gaps

logic: the max size of gaps is max half of the input
example: "a|b|c|d", 3 gaps for input of uneven size of 7
(int)(7/2)  yields 3
cast to int rounds down by simply cutting of the fractional part
*/
static dbj_array  * dbj_str_gaps_locations(
	const char text_[],
	const char boundary_[]
)
{
	int marks_len = 1 + (int)(strlen(text_) / 2);
	// no need to free
	int * marks = dbj_stack_alloc(int, marks_len);
	memset(marks, 0, marks_len);
	int gap_count = 0;

	const char *str = text_;
	const char *begin = &str[0];
	const char *target = boundary_;
	const int   target_len = strlen(target);
	const int   gap_len = strlen(boundary_);
	const char *result = str;
	size_t      gap_location = 0;
	while ((result = strstr(result, target)) != NULL)
	{
		gap_location = result - begin;
		// this would mean boundary is prefix which is not allowed
		assert(gap_location > 0);

		marks[gap_count] = gap_location;
		// increment marks walker
		++gap_count;
		// Increment result, otherwise we'll find target at the same location
		++result;
	}
	dbj_array * p = 0;
	dbj_array_make(p, marks, gap_count, int);
	// we have made a structure that holds size of int
	// count of array of ints and void * to the array of ints
	return p;
}

/*
remove all spaces found and remove potential 
prefix/suffix boundaries
*/
static char * prepare_to_sawmill(const char text_[], const char boundary_[])
{
	char * finaly_the_text_ = 0;
	char * new_text_ = dbj_str_remove_all(text_, " ");
	if (!new_text_) goto safe_exit;;

	char *text_front, *text_back;
	int failure;
	if ((failure = dbj_str_remove_prefix_suffix(
		&text_front, &text_back,
		new_text_, boundary_)))
	{
		printf("\n\ndbj_str_remove_prefix_suffix error message:%s\n", strerror(failure));
		goto safe_exit;
	}
	finaly_the_text_ = dbj_strndup(text_front, text_back - text_front); // can be NULL

safe_exit:
	if (new_text_) free(new_text_);
	return finaly_the_text_; // can be NULL
}

// return rezult_count > 0 
unsigned int dbj_str_sawmill(
	char ** rezult,		//--rezulting array of strings
	const char    text_[],	//--input text
	const char    boundary_[]	//--what to cut out
)
{
	int  rezult_count = 0;	//--count of the rezulting array of strings
	errno = 0;
	dbj_array * gaps_ = 0;

	// Errors in calling logic
	// are the input strings both zero limited?
	if (NULL == strchr(text_, 0)) { errno = EINVAL; goto safe_exit; }
	if (NULL == strchr(text_, 0)) { errno = EINVAL; goto safe_exit; }
	// boundary not found ?
	if (NULL == strstr(text_, boundary_)) { errno = EINVAL; goto safe_exit; }
	// is boundary larger or equal v.s. the text_ ?
	if (strlen(boundary_) >= strlen(text_)) { errno = EINVAL; goto safe_exit; }

// remove the spaces and remove 
// potential prefix/suffix boundaries
	char * sawmill_text_ = prepare_to_sawmill(text_, boundary_);
		if (!sawmill_text_) { errno = EINVAL; goto safe_exit; }

#if defined(_DEBUG) || defined(DEBUG)
	// in debug build check if prepare_to_sawmill() has done a good job
	// is boundary prefix to the input?
	if (is_dbj_str_prefix(sawmill_text_, boundary_)) { errno = EINVAL; goto safe_exit; }
	// is boundary suffix to the input?
	if (is_dbj_str_suffix(sawmill_text_, boundary_)) { errno = EINVAL; goto safe_exit; }
#endif

	// do the fuck all, in one go
	rekurzor(dbj_sll_tls_head(), sawmill_text_, boundary_, strlen(boundary_));
	// the global sll is the result
	goto safe_exit;

	// find and return locations of boundaries
	gaps_ = dbj_str_gaps_locations(sawmill_text_, boundary_);
	// any system failure or are there any gaps located?
	if ( (! gaps_) || (gaps_->count_ < 1)) { errno = EINVAL; goto safe_exit; }

	// rezult array will have gaps_count + 1 elements
	rezult_count = 1 + dbj_array_count(gaps_) ;
	(*rezult) = dbj_calloc(char *, rezult_count);
	int * gaps
		= dbj_array_cast(gaps_, int);
	int j = 0,
		from_pos = 0,
		to_pos = 0;
	const int gap_size = strlen(boundary_);
	int word_len = 0;
	// one less than number of rezulting elements
	for (j = 0; j < dbj_array_count(gaps_); ++j ) {
		to_pos = gaps[j];
		word_len = to_pos - from_pos;
		// word_len == 0 should not happen
		assert(word_len > 0);
			(rezult)[j] = dbj_strndup(&text_[from_pos], word_len);
		from_pos = to_pos + gap_size;
	} 
	// fetch the last element
	(rezult)[j] = dbj_strndup(&text_[from_pos], word_len);
	// has every element been done?
	safe_exit:
	if (sawmill_text_) { free(sawmill_text_); sawmill_text_ = 0; }
	if (gaps_) {dbj_array_free(gaps_); gaps_ = 0;}
	return rezult_count;
}

static void dbj_str_sawmill_release_rezult( size_t count, char * rezult []) {
	if (rezult) {
		for (size_t n = 0; n < count; ++n) {
			if (rezult[n]) {
				free(rezult[n]); rezult[n] = 0;
			}
		}
		// free(rezult);
		rezult = 0;
	}
}

extern const char large_text[];

void test_dbj_str_sawmill()
{
	char ** rezult = (char **)malloc( sizeof(char **));
	const  char * text_ = "one two one three one four one",	*boundary = "one";

	int rezult_size = dbj_str_sawmill( rezult, large_text, boundary);
	if (errno) {
		printf("\n\ndbj str sawmill error message:%s\n", strerror(errno));
	}
	else {
		printf("\nDBJ SLL dump after dbj_str_sawmill()");
		dbj_sll_foreach(dbj_sll_tls_head(), dbj_sll_node_dump_visitor);
		printf("\n");
		dbj_sll_erase(dbj_sll_tls_head());

		return;

		// print the result
		printf("\n\ndbj str sawmill rezulted in %d strings\n", rezult_size);
		for (int j = 0; j < rezult_size ; ++j) {
			if (rezult[j]) {
				printf("\n%3.3d %s", j, rezult[j]);
			}
		}
	}

	dbj_str_sawmill_release_rezult(rezult_size,rezult);
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