
#include "dbj_sawmill.h"


#pragma region dbj micro lib
/*
strdup and strndup are defined in POSIX compliant systems as :

char *strdup(const char *str);
char *strndup(const char *str, size_t len);
*/
static char * dbj_strdup(const char *s)
{
	char *d = (char *)malloc(sizeof(char) * (strlen(s) + 1));
	if (d == NULL) { errno = ENOMEM; return NULL; }
	strcpy(d, s);
	return d;
}

/*
The strndup() function copies at most len characters from the string str
always null terminating the copied string.
input does not have to be zero terminated
*/
static char * dbj_strndup(const char str_[], const size_t n)
{
	assert(str_);
	char *result = (char *)calloc(n+1, sizeof(char));
	if (result == NULL) { errno = ENOMEM; return NULL; } 
	strncpy(result, str_, n);
	result[n] = '\0';
	return result;
}

/*
return the index of the substring found or -1
*/
static int dbj_substr_pos(const char str_[], const char sub_[]) {
	char* pos = strstr(str_, sub_);
	return (pos != NULL ? pos - str_ : -1);
}

#if defined(_DEBUG) || defined(DEBUG)
/* is boundary prefix to the input? */
static bool is_dbj_str_prefix(const char str_[], const char pfx_[]) {
	return (0 == dbj_substr_pos(str_, pfx_));
}
/* is boundary suffix to the input? */
static bool is_dbj_str_suffix(const char str_[], const char sfx_[])
{
	assert(str_);
	assert(sfx_);

	size_t str_len_ = strlen(str_);
	size_t pfx_len_ = strlen(sfx_);

	assert(str_len_);
	assert(pfx_len_);

	size_t presumed_suffix_location = str_len_ - pfx_len_;
	char * presumed_suffix = (char*)&str_[presumed_suffix_location];
	if (0 == dbj_substr_pos(presumed_suffix, sfx_))
	{
		/* boundary found to be suffix */
		return true;
	}
	return false;
}

#endif /* #if defined(_DEBUG) || defined(DEBUG) */
/*
remove chars given in the string arg
return the new string
user has to free() eventually
both arguments must be zero limited
*/
static char * dbj_str_remove_all(const char * str_, const char * chars_to_remove_)
{
	assert(strchr(str_, 0));
	assert(strchr(chars_to_remove_, 0));

	size_t str_size = strlen(str_);
	/* on the stack */
#ifndef _MSC_VER
	char * vla_buff_ = (char *)alloca(str_size);
#else
	char * vla_buff_ = (char *)_malloca(str_size);
#endif
	char * vla_buff_first = &vla_buff_[0];
	char * vla_buff_last = &vla_buff_[str_size - 1];
	char * buff_p = vla_buff_first;
	char * j = 0;
	for (j = (char *)str_; *j != (char)0; j++) {
		if (0 == strchr(chars_to_remove_, *j))
		{
			*buff_p = *j; buff_p++;
		}
		/* safety measure  */
		if (buff_p == (1 + vla_buff_last)) break;
	}
	*buff_p = 0;
	return dbj_strndup(vla_buff_, buff_p - vla_buff_);
}

/*
return 0 on OK, errno on error
after new_front and new_back are moved if prefix/sufix is found
*/
int dbj_str_remove_prefix_suffix(
	char ** new_front,
	char ** new_back,
	const char text_[],
	const char boundary_[]
) {
	assert(text_);
	assert(boundary_);
	if (text_[0] == '\0')  return errno = EINVAL;
	if (boundary_[0] == '\0')  return errno = EINVAL;

	const size_t text_len_ = strlen(text_);
	const size_t boundary_len_ = strlen(boundary_);

	if (boundary_len_ >= text_len_) return errno = EINVAL;

	*new_front = (char *)&text_[0];
	*new_back = (char *)&text_[text_len_ - 1];

	/* is boundary prefix to the input? */
	if (0 == dbj_substr_pos(text_, boundary_)) {
		/* it is */
		*new_front = (char *)&text_[boundary_len_];
	}
	/* is boundary suffix to the input? */
	{
		size_t presumed_suffix_location = text_len_ - boundary_len_;
		char * presumed_suffix = (char*)&text_[presumed_suffix_location];
		if (0 == dbj_substr_pos(presumed_suffix, boundary_))
		{
			/* boundary found to be suffix */
			*new_back = (char *)&text_[presumed_suffix_location];
		}
	}
	return 0;
}

#pragma endregion dbj micro lib

/*
The core of the trick
*/
static void rekurzor(
	dbj_sll_node ** sll_head,
	const char text_[],
	const char boundary_[],
	const size_t gap_size
)
{
	if (!text_) return;
	int gap = dbj_substr_pos(text_, boundary_);
	if (-1 == gap) {
		/* the last chunk */
		dbj_sll_append(*sll_head, text_);
		return;
	}
	/* ovo moze mnogo bolje
	 but "premature optimization is the root of all evil" */
	char * chunk = dbj_strndup(text_, gap);
	dbj_sll_append(*sll_head, chunk);
	free(chunk);
	/* rekurziraj dalje */
	rekurzor(sll_head, &(text_)[gap + gap_size], boundary_, gap_size);
}

/*
remove all spaces found and remove potential
prefix/suffix boundaries
return the prepared text made on the heap
*/
static char * prepare_to_sawmill(const char text_[], const char boundary_[])
{
	char * finaly_the_text_ = 0;
	char * new_text_ = dbj_str_remove_all(text_, " ");
	if (!new_text_) goto safe_exit;

	char *text_front, *text_back;
	int failure;
	if ((failure = dbj_str_remove_prefix_suffix(
		&text_front, &text_back,
		new_text_, boundary_)))
	{
		printf("\n\ndbj_str_remove_prefix_suffix error message:%s\n", strerror(failure));
		goto safe_exit;
	}
	finaly_the_text_ = dbj_strndup(text_front, text_back - text_front); /* can be NULL */

safe_exit:
	if (new_text_) free(new_text_);
	return finaly_the_text_; /* can be NULL */
}

int dbj_sawmill(
	dbj_sll_node * the_sll,
	const char    text_[],	
	const char    boundary_[]	
)
{
	errno = 0;
	char * sawmill_text_ = 0;
	/* check for the calling sanity
	 are the input strings both zero limited? */
	if (NULL == strchr(text_, 0)) { errno = EINVAL; goto safe_exit; }
	if (NULL == strchr(text_, 0)) { errno = EINVAL; goto safe_exit; }
	/* boundary not found ? */
	if (NULL == strstr(text_, boundary_)) { errno = EINVAL; goto safe_exit; }
	/* is boundary larger or equal v.s. the text_ ? */
	if (strlen(boundary_) >= strlen(text_)) { errno = EINVAL; goto safe_exit; }

	/* remove the spaces and remove 
	   potential prefix/suffix boundaries */
	     sawmill_text_ = prepare_to_sawmill(text_, boundary_);
	if (!sawmill_text_) { if ( ! errno ) errno = EINVAL; goto safe_exit; }
	if (errno) goto safe_exit;

#if defined(_DEBUG) || defined(DEBUG)
	/* in debug build check if prepare_to_sawmill() has done a good job
	   is boundary prefix to the input? */
	if (is_dbj_str_prefix(sawmill_text_, boundary_)) { errno = EINVAL; goto safe_exit; }
	/* is boundary suffix to the input? */
	if (is_dbj_str_suffix(sawmill_text_, boundary_)) { errno = EINVAL; goto safe_exit; }
#endif

	/* remove nodes from the thread global SLL */
	dbj_sll_erase(the_sll);
#if defined(_DEBUG) || defined(DEBUG)
	assert(is_dbj_sll_empty(the_sll));
	assert(is_dbj_sll_head(the_sll));
#endif
	/* do it all, in one go */
	rekurzor(
		&the_sll,
		sawmill_text_, 
		boundary_, 
		strlen(boundary_)
	);
	/* the thread global sll now holds the result */
safe_exit:
	if (sawmill_text_) { free(sawmill_text_); sawmill_text_ = 0; }
	return errno;
}

