


#ifndef __KUM_H
#define __KUM_H


#include <kulib/kuconv.h>
#include <kuconv_mbcs.h >
#include <kuconv_int.h >

enum
{
	KUM_UNDEFINED_CONVERTER = -1,
	KUM_SBCS = 0,
	KUM_DBCS,
	KUM_MBCS
};

enum
{
    KUM_STATE_FLAG_DIRECT = 1,
    KUM_STATE_FLAG_SURROGATES,

    KUM_STATE_FLAG_READY = 0x10
};

typedef struct __kum_mapping
{
    ucs4_t u;
    union 
	{
		uint32_t index;
		uint8_t bytes[ 4 ];
    } b;
//    int8_t ulen;
	int8_t blen;
	int8_t f;
	int8_t move_flag;
} kum_mapping;

typedef struct __kum_table
{
    kum_mapping* map;
    int32_t map_allocated;
	int32_t map_len;

/*
    ucs4_t* code_points;
    int32_t cp_allocated;
	int32_t cp_len;
*/

    uint8_t* bytes;
    int32_t bytes_allocated;
	int32_t bytes_len;

    int32_t* reverse_map;

    uint8_t unicode_mask;
    int sorted;
} kum_table;

typedef struct __kum_states
{
	int32_t states[ MBCS_MAX_STATE_COUNT ][ 0x100 ];
	uint8_t flags[ MBCS_MAX_STATE_COUNT ];
	uint32_t offsets[ MBCS_MAX_STATE_COUNT ];
	int states_count;

	int32_t to_unicode_units_count;
} kum_states;

typedef struct __kum_file
{

	char name[ KUCONV_MAX_CONVERTER_NAME_LENGTH ];
	
	int min_char_len;
	int max_char_len;

    uint8_t sub_char[ KUCONV_MAX_SUBCHAR_LEN ];
    int8_t sub_char_len;
    uint8_t sub_char1;

	int8_t conv_type;
	
	kum_states states;

	kum_table* table;
} kum_file;


kum_file* kum_open( const char* filename );

void kum_add_state( kum_states* states, const char* s );
void kum_process_states( kum_file* kum );
int32_t kum_count_chars( kum_states* states, const uint8_t* bytes, int32_t len );


#endif /* __KUM_H */
