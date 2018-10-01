#pragma once
#include "dbj.h"

/*
Singly Linked List of strings
Specialised non-generic , one head per thread solution
Each node has a key which is a hash of the string it also has
*/

typedef struct dbj_sll_node dbj_sll_node;

typedef struct dbj_sll_node  {
	unsigned long key;
	// the above is usually the hash of the data bellow
	// this it is unsigned long
	// se the dbj_hash and the link in its comment
	char * data;
	dbj_sll_node * next;
} dbj_sll_node ;

////////////////////////////////////////////////////////
// special single node aka the "HEAD"
// Thread Local Storage (TLS) solution
static dbj_sll_node * dbj_sll_tls_head() {
	dbj_thread_local dbj_sll_node 
		dbj_sll_tls_head_ = { -1, { "HEAD" }, 0 };
	dbj_sll_node * the_head_ptr 
		= &dbj_sll_tls_head_;
	return the_head_ptr;
}

/*
all of the DBJ SLL functions bellow operate on a single head
which is thread global as defined above
*/

////////////////////////////////////////////////////////
static bool dbj_sll_empty()
{
	return (dbj_sll_tls_head()->next == 0);
}
////////////////////////////////////////////////////////
static dbj_sll_node * dbj_sll_node_new() 
{
	dbj_sll_node * new_ 
		= (dbj_sll_node *)malloc(sizeof(dbj_sll_node));
	if (new_) {
		int key_	= 0;
		new_->data	= 0;
		new_->next	= 0;
	}
	else {
		errno = ENOMEM;
	}
		return new_;
}
////////////////////////////////////////////////////////
static dbj_sll_node * dbj_sll_tail()
{
	// get the head
	dbj_sll_node * walker_ = dbj_sll_tls_head();
	while (walker_) {
		if (!walker_->next) break;
		walker_ = walker_->next;
	}
	return walker_;
}
////////////////////////////////////////////////////////
/// erase the whole list
static void dbj_sll_erase()
{
	dbj_sll_node * current_ = dbj_sll_tls_head()->next ;
	dbj_sll_node * temp_ = 0;
	while (current_) {
		temp_ = current_->next;
		free(current_->data);
		free(current_);
		current_ = temp_;
	};
	// make the head aware there are no nodes left
	dbj_sll_tls_head()->next = 0;
}
////////////////////////////////////////////////////////
///  find by key
static dbj_sll_node * dbj_sll_find( int k_)
{
	// get and skip the head
	dbj_sll_node * walker_ = dbj_sll_tls_head()->next;
	while (walker_) {
		if (walker_->key == k_) return walker_;
		walker_ = walker_->next;
	}
	// not found
	return NULL;
}
////////////////////////////////////////////////////////
///  visitor function to each node
///  visitation is stopped when 
///  visitor returns true
///  remember: head is never used
///  it is just an anchor
///  return the pointer to the last node visited
///  visitor function signature
/// 
///  bool (*visitor)(dbj_sll_node *)
/// 
/// NOTE: most (if not all) SLL operations 
/// can be implemented as visitors
/// 
static dbj_sll_node * dbj_sll_foreach
   ( bool (*visitor)(dbj_sll_node *))
{
	// get and skip the head
	dbj_sll_node * walker_ = dbj_sll_tls_head()->next;
	while (walker_) {
		if ( visitor ( walker_ )) return walker_ ;
		walker_ = walker_->next;
	}
	// not found
	return NULL;
}
////////////////////////////////////////////////////////

static size_t dbj_sll_count()
{
	size_t count_ = 0;
	// get and skip the head
	dbj_sll_node * walker_ = dbj_sll_tls_head()->next ;
	while (walker_) {
		++count_;
		walker_ = walker_->next;
	}
	return count_;
}


////////////////////////////////////////////////////////
// return the newly made and appended node
// str argument is copied and thus has to 
// be freed sometimes later
// key is generated as the hash() of the str_argument
static dbj_sll_node * 
		dbj_sll_append( const char * str_)
{
	dbj_sll_node * new_node = dbj_sll_node_new();
	new_node->data = dbj_strdup(str_);
	new_node->key = dbj_hash( (unsigned char *)str_);

	dbj_sll_node * tail_node = dbj_sll_tail();
	tail_node->next = new_node ;
	return new_node;
}

// sll visitors
static bool dbj_sll_node_dump_visitor(dbj_sll_node * node_) {

	printf("\n\nKey: %ld", node_->key);
	printf("\nStr: ");
	dbj_dump_charr_arr(strlen(node_->data), node_->data, false);
	printf("\nNext: %p", node_->next);
	// return false as a signal NOT to stop
	return false;
}
/*
this is where we can clearly see the disdvantage of singly linked lists
vs doubly linked lists.

but SLL's simplicity more than compesates for this
SLL lists are short and todays machines are fast
so the algorithms are the one bellow are OK in normal situations
*/
static bool dbj_sll_remove_tail_visitor(dbj_sll_node * node_) 
{
	if (!node_->next) return true; // stop

	// is the next one the tail?
	if (!node_->next->next) {
	// next one is the tail
		free(node_->next->data);
		free(node_->next);
		node_->next = 0;
		return true; // stop
	}
	return false; // proceed
}

// return the new tail or 0 if list is empty
static dbj_sll_node * dbj_sll_remove_tail()
{ 
	if (dbj_sll_empty()) return 0;
	return dbj_sll_foreach(dbj_sll_remove_tail_visitor);
}

///////////////////////////////////////////////////////////////////////////////////////////

static void test_dbj_sll()
{
	dbj_sll_append("ONE");
	dbj_sll_append("TWO");
	dbj_sll_append("THREE");

	printf("\nDBJ SLL dump");
	dbj_sll_foreach(dbj_sll_node_dump_visitor);
	printf("\n");

	assert(0 == strcmp(dbj_sll_remove_tail()->data, "TWO"));

	dbj_sll_erase();

	printf("\nHead after SLL erasure");
	dbj_sll_node_dump_visitor(dbj_sll_tls_head());

	assert(true == dbj_sll_empty());

	unsigned long k1 = dbj_sll_append("Abra")->key;
	unsigned long k2 = dbj_sll_append("Ka")->key;
	unsigned long k3 = dbj_sll_append("Dabra")->key;

	dbj_sll_node * node_ = dbj_sll_find(k2);
	assert(0 == strcmp(node_->data, "Ka"));

	system("pause");
}