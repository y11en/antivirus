

/* Hashtable implemented as double hash. */

#include "kuhash.h"
#include "kualloc.h"
#include "kuconv_int.h"

#include <string.h>

static const int32_t hash_length_primes[ ] =
{
    13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191, 16381, 32749,
    65521, 131071, 262139, 524287, 1048573, 2097143, 4194301, 8388593,
    16777213, 33554393, 67108859, 134217689, 268435399, 536870909,
    1073741789, 2147483647 /*, 4294967291 */
};

#define PRIMES_LENGTH	( sizeof(hash_length_primes) / sizeof(hash_length_primes[0]) )

static const float resize_policy_ratio_table[ 3 ][ 2 ] =
{
    { 0.0F, 0.5F },
    { 0.1F, 0.5F },
    { 0.0F, 1.0F }
};


#define HASH_DELETED    ( (int32_t) 0x80000000 )
#define HASH_EMPTY      ( (int32_t) HASH_DELETED + 1 )
#define IS_EMPTY_OR_DELETED( h )	( ( h ) < 0 )



typedef struct __kuhash
{
	int32_t used;
	int32_t length;

	int prime_index;

	hash_elem* elems;

	int32_t low_water_mark;
	int32_t high_water_mark;

	float   low_water_ratio;
	float   high_water_ratio;

	kuhash_proc_t hash_proc;
	kuhash_compare_proc_t compare_proc;
} kuhash;


static int _hash_allocate( kuhash_t hash, int prime_index );
static int _hash_rehash( kuhash_t hash );
static hash_elem* _hash_find( kuhash_t hash, kuhash_key_t key, int32_t hashcode );


int32_t kuhash_chars( kuhash_key_t key )
{
    int32_t hash = 0;
    const char* s = (const char*) key;
    if( s != NULL )
	{
        int32_t len = (int32_t) strlen( s );
        int32_t inc = ( ( len - 32 ) / 32 ) + 1;
        const char* limit = s + len;
        while( s < limit )
		{
            hash = ( hash * 37 ) + *s;
            s += inc;
        }
    }
    return hash;
}

int kuhash_compare_chars( kuhash_value_t key1, kuhash_value_t key2 )
{
    const char *s1 = (const char*) key1;
    const char *s2 = (const char*) key2;
    
	if( s1 == s2 )
	{
        return 1;
    }

    if( s1 == NULL || s2 == NULL )
	{
        return 0;
    }

    while( *s1 != '\0' && *s1 == *s2 )
	{
        ++s1;
        ++s2;
    }

    return *s1 == *s2;
}


int kuhash_create( kuhash_proc_t hash_proc, kuhash_compare_proc_t compare_proc,
			int32_t size, kuhash_resize_policy_t policy, kuhash_t* phash )
{
	kuhash_t hash;
	int prime_index;
	int ret;

	if( size < 0 )
		prime_index = 3;
	else
	{
		prime_index = 0;
		while( prime_index < ( PRIMES_LENGTH - 1 )
			&& hash_length_primes[ prime_index ] < size )
		{
			++prime_index;
		}
	}


	if( ( hash = ku_malloc( sizeof( kuhash ) ) ) == NULL )
		return KU_MEMORY_ALLOCATION_ERROR;

	hash->hash_proc = hash_proc;
	hash->compare_proc = compare_proc;

	kuhash_set_resize_policy( hash, policy );

	if( ( ret = _hash_allocate( hash, prime_index ) ) != KU_OK )
	{
		ku_free( hash );
		return ret;
	}

	*phash = hash;

	return KU_OK;
}


int kuhash_destroy( kuhash_t hash )
{
	ku_free( hash->elems );
	hash->elems = NULL;

	ku_free( hash );

	return KU_OK;
}


int kuhash_set_resize_policy( kuhash_t hash, kuhash_resize_policy_t policy )
{
    hash->low_water_ratio = resize_policy_ratio_table[ policy ][ 0 ];
	hash->high_water_ratio = resize_policy_ratio_table[ policy ][ 1 ];

	return KU_OK;
}

int32_t kuhash_size( kuhash_t hash )
{
	return hash->used;
}

const kuhash_elem_t* kuhash_next_elem( kuhash_t hash, int32_t* pos )
{
    int32_t i;

    for( i = *pos + 1; i < hash->length; ++i )
	{
        if( !IS_EMPTY_OR_DELETED( hash->elems[ i ].hashcode ) )
		{
            *pos = i;
            return &( hash->elems[i] );
        }
    }

    return NULL;
}

const kuhash_elem_t* kuhash_find( kuhash_t hash, kuhash_key_t key )
{
    const hash_elem* elem;

    elem = _hash_find( hash, key, hash->hash_proc( key ) );
    return IS_EMPTY_OR_DELETED( elem->hashcode ) ? NULL : elem;
}

int kuhash_remove_elem( kuhash_t hash, const kuhash_elem_t* elem )
{
    if( IS_EMPTY_OR_DELETED( elem->hashcode ) )
		return 0;

	--hash->used;

	( (kuhash_elem_t*) elem )->hashcode = HASH_DELETED;
	( (kuhash_elem_t*) elem )->key = NULL;
	( (kuhash_elem_t*) elem )->value = NULL;

	return 1;
}

int kuhash_set( kuhash_t hash, kuhash_key_t key, kuhash_value_t value )
{
	int32_t hashcode;
	hash_elem* elem;

    if( hash->used > hash->high_water_mark )
	{
        _hash_rehash( hash );
    }

	if( hash->used == hash->length )
	{
		return KU_MEMORY_ALLOCATION_ERROR;
	}

	hashcode = hash->hash_proc( key );

	elem = _hash_find( hash, key, hashcode );
	/* KUCONV_ASSERT( elem != NULL ); */

	elem->hashcode = hashcode & 0x7FFFFFFF;
	elem->key = key;
	elem->value = value;

	++hash->used;
	
	return KU_OK;
}

static int _hash_allocate( kuhash_t hash, int prime_index )
{
	hash_elem* cur;
	hash_elem* end;

	hash->prime_index = prime_index;
	hash->length = hash_length_primes[ prime_index ];

	hash->elems = (hash_elem*) ku_malloc( sizeof(hash_elem) * hash->length );
	if( hash->elems == NULL )
		return KU_MEMORY_ALLOCATION_ERROR;

	cur = hash->elems;
	end = hash->elems + hash->length;
	while( cur < end )
	{
		cur->hashcode = HASH_EMPTY;
		cur->key = NULL;
		cur->value = NULL;
		++cur;
	}

	hash->used = 0;
	hash->low_water_mark = (int32_t) ( hash->length * hash->low_water_ratio );
	hash->high_water_mark = (int32_t) ( hash->length * hash->high_water_ratio );

	return KU_OK;
}

static int _hash_rehash( kuhash_t hash )
{

    hash_elem* old_elems = hash->elems;
    int32_t old_length = hash->length;
    int32_t new_prime_index = hash->prime_index;
    int32_t i;
	int ret;

    if( hash->used > hash->high_water_mark )
	{
        if( ++new_prime_index >= PRIMES_LENGTH )
            return KU_OK;
    }
	else if( hash->used < hash->low_water_mark )
	{
        if( --new_prime_index < 0 )
            return KU_OK;
    }
	else
	{
        return KU_OK;
    }

    ret = _hash_allocate( hash, new_prime_index );

    if( ret != KU_OK ) {
        hash->elems = old_elems;
        hash->length = old_length;
        return ret;
    }

    for( i = old_length - 1; i >= 0; --i )
	{
        if( !IS_EMPTY_OR_DELETED( old_elems[i].hashcode ) ) {
            hash_elem* e = _hash_find( hash, old_elems[i].key, old_elems[i].hashcode );
            /* KUCONV_ASSERT( e != NULL ); */
            /* KUCONV_ASSERT( e->hashcode == HASH_EMPTY ); */
            e->key = old_elems[i].key;
            e->value = old_elems[i].value;
            e->hashcode = old_elems[i].hashcode;
            ++hash->used;
        }
    }

    ku_free( old_elems );

	return KU_OK;
}


static hash_elem* _hash_find( kuhash_t hash, kuhash_key_t key, int32_t hashcode )
{

    int32_t first_deleted = -1;
    int32_t index, start_index;
    int32_t jump = 0;
    int32_t elem_hashcode;

	hashcode &= 0x7FFFFFFF;	/* must be positive */
    start_index = index = ( hashcode ^ 0x4000000 ) % hash->length;

    do
	{
        elem_hashcode = hash->elems[ index ].hashcode;

        if( elem_hashcode == hashcode )
		{
            if( (*hash->compare_proc)( key, hash->elems[ index ].key ) )
			{
                return &( hash->elems[ index ] );
            }
        }
		else if( IS_EMPTY_OR_DELETED( elem_hashcode ) )
		{
			if( elem_hashcode == HASH_EMPTY )
				break;

			if( first_deleted < 0 )
				first_deleted = index;
		}

        if( jump == 0 )
		{
            jump = ( hashcode % ( hash->length - 1 ) ) + 1;
		}

		index = ( index + jump ) % hash->length;
    }
	while( index != start_index );

    if( first_deleted >= 0 )
	{
        index = first_deleted;
    }
	else if( elem_hashcode != HASH_EMPTY )
	{
        /* THIS WILL NEVER HAPPEN */
        /* KUCONV_ASSERT( FALSE ); */
        return NULL;
    }

    return &( hash->elems[index] );
}
