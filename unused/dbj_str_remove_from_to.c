#pragma once

#include "dbj.h"

#pragma warning( push )
#pragma warning( disable : 4018 ) 
/**
 * Extracts a selection of string and return a new string or NULL.
 * It supports both negative and positive indexes.
 */
 char * dbj_str_remove_from_to(char str[], int slice_from, int slice_to)
{
    if (str[0] == '\0')
        return NULL;

    size_t str_len = 0, buffer_len = 0;

    /* for negative indexes "slice_from" must be < "slice_to" */
    if (slice_to < 0 && slice_from < slice_to) {
        str_len = strlen(str);

        /* if "slice_to" goes beyond limits */
        if (abs(slice_to) > str_len - 1)
            return NULL;

        /* if "slice_from" goes beyond limits */
        if (abs(slice_from) > str_len)
            slice_from = (-1) * str_len;

        buffer_len = slice_to - slice_from;
        str += (str_len + slice_from);

    /* for positive indexes "slice_from" must be before "slice_to" */
    } else if (slice_from >= 0 && slice_to > slice_from) {
        str_len = strlen(str);

        /* if "slice_from" goes beyond permissible limits */
        if (slice_from > str_len - 1)
            return NULL;

        buffer_len = slice_to - slice_from;
        str += slice_from;

    /* otherwise, returns NULL */
    } else
        return NULL;

    //char * buffer = (char *)calloc(buffer_len, sizeof(char));
    // strncpy(buffer, str, buffer_len);
    return dbj_strndup(str, buffer_len) ;
}

#pragma warning( pop )

#ifndef BUFSIZ
#define BUFSIZ 512
#endif

static char static_buffer[BUFSIZ] ;

static char *
dbj_str_test_from_to_driver (char str[], int slice_from, int slice_to) 
{
    char * rezult = dbj_str_remove_from_to(str, slice_from, slice_to) ;
    
    memset( static_buffer, BUFSIZ, 0);
    
    if ( rezult ) {
       strncpy( static_buffer, rezult, BUFSIZ ) ;
		   free( rezult ) ;
    } else {
        return "" ; // so that strcmp does not do SEGV on NULL return
    }
    return & static_buffer[0] ;

}

 void test_dbj_str_remove_from_to()
{
    char str[] = "abcdefghijkl";

    assert(strcmp( dbj_str_test_from_to_driver(str, -3, -10), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver(str, -1, -2), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver(str, -1, 0), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver(str, 1, 0), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver(str, 5, 4), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver(str, 0, 0), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver(str, 10, 10), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver(str, -2, -2), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver(str, -20, -12), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver(str, -20, -13), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver(str, 12, 13), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver(str, 12, 20), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver("", 1, 2), "" ) == 0 );
    assert(strcmp( dbj_str_test_from_to_driver("", -2, -1), "" ) == 0 );
    assert(strcmp(dbj_str_test_from_to_driver(str, -3, -1), "jk") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, -8, -3), "efghi") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, -10, -9), "c") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, -2, -1), "k") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, -15, -1), "abcdefghijk") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, -12, -2), "abcdefghij") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, -15, -8), "abcd") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, -15, -11), "a") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, 1, 3), "bc") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, 11, 100), "l") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, 2, 4), "cd") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, 3, 6), "def") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, 0, 1), "a") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, 4, 6), "ef") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, 1, 2), "b") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, 0, 3), "abc") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, 0, 11), "abcdefghijk") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, 2, 10), "cdefghij") == 0);
    assert(strcmp(dbj_str_test_from_to_driver(str, 0, 50), "abcdefghijkl") == 0);
}
