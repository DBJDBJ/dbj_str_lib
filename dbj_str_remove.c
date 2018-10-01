#pragma once
#include "dbj.h"
// remove chars given in the string arg
// return the new string
// user has to free() eventually
// both arguments must be zero limited
char * dbj_str_remove_all ( const char * str_, const char * chars_to_remove_ )
{
	assert(strchr(str_, 0));
	assert(strchr(chars_to_remove_, 0));

	size_t str_size = strlen(str_);
	// on the stack
#ifndef _MSC_VER
	char * vla_buff_  = (char *)alloca ( str_size ) ;
#else
	char * vla_buff_ = (char *)_malloca(str_size);
#endif
	char * vla_buff_first	= & vla_buff_[0] ;
	char * vla_buff_last	= & vla_buff_[str_size - 1] ;
	char * buff_p			= vla_buff_first;
	for  (char * j = (char *)str_; *j != (char)0; j++) {
		if (0 == strchr(chars_to_remove_, *j))
		{
			*buff_p = *j; buff_p++;
		}
		// safety measure
		if (buff_p == (1 + vla_buff_last)) break; 
	}
	*buff_p = 0; 
	return dbj_strndup(vla_buff_, buff_p - vla_buff_);
}

void dbj_test_str_remove() 
{
char * specimen_ = "    ABRA    KA     DABRA    ";
assert(0 == strcmp("ABRAKADABRA", dbj_str_remove_all(specimen_, " ")));
assert(0 == strcmp("KD", dbj_str_remove_all(specimen_, "ABR ")));
}