

#ifndef __KUHASH_H
#define __KUHASH_H

#include <kulib/ku_defs.h>

KU_CDECL_BEGIN

typedef void* kuhash_key_t;
typedef void* kuhash_value_t;

typedef int32_t (*kuhash_proc_t) ( kuhash_key_t );
typedef int (*kuhash_compare_proc_t) ( kuhash_value_t, kuhash_value_t );

//struct __kuhash;
typedef struct __kuhash* kuhash_t;

typedef enum
{
    GROW_ONLY,
    GROW_AND_SHRINK,
    FIXED
} kuhash_resize_policy_t;


typedef struct __hash_elem
{
	int32_t hashcode;
	kuhash_key_t key;
	kuhash_value_t value;
} kuhash_elem_t, hash_elem;


int32_t kuhash_chars( kuhash_key_t key );
int kuhash_compare_chars( kuhash_value_t key1, kuhash_value_t key2 );


int kuhash_create( kuhash_proc_t hash_proc, kuhash_compare_proc_t compare_proc,
			int32_t size, kuhash_resize_policy_t policy, kuhash_t* hash );
int kuhash_destroy( kuhash_t hash );

int kuhash_set_resize_policy( kuhash_t hash, kuhash_resize_policy_t policy );

int32_t kuhash_size( kuhash_t hash );

int kuhash_set( kuhash_t hash, kuhash_key_t key, kuhash_value_t value );

int kuhash_remove( kuhash_t hash, kuhash_key_t key );

const kuhash_elem_t* kuhash_next_elem( kuhash_t hash, int32_t* pos );

const kuhash_elem_t* kuhash_find( kuhash_t hash, kuhash_key_t key );

int kuhash_remove_elem( kuhash_t hash, const kuhash_elem_t* elem );


KU_CDECL_END


#endif /* __KUCONV_HASH_H */
