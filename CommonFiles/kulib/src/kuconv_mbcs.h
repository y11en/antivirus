

#ifndef __KUCONV_MBCS_H
#define __KUCONV_MBCS_H

#include <kulib/ku_defs.h>


/* forward declaration */
struct __conv_info;
typedef struct __conv_info conv_info;



enum mbcs_mapping_type
{
	MBCS_ROUNDTRIP = 0,
	MBCS_FALLBACK,
	MBCS_REVERSE_FALLBACK
};


#define MBCS_MAX_STATE_COUNT	0x7f


enum {
    MBCS_STATE_VALID_DIRECT_16,
    MBCS_STATE_VALID_DIRECT_20,

    MBCS_STATE_FALLBACK_DIRECT_16,
    MBCS_STATE_FALLBACK_DIRECT_20,

    MBCS_STATE_VALID_16,
	MBCS_STATE_VALID_16_PAIR,

    MBCS_STATE_UNASSIGNED,
    MBCS_STATE_ILLEGAL,

    MBCS_STATE_CHANGE_ONLY
};

#define MBCS_ENTRY_TRANSITION( state, offset ) \
		(int32_t) ( ( (int32_t) (state) << 24L ) | (offset ) )
#define MBCS_ENTRY_TRANSITION_SET_OFFSET( entry, offset ) \
		(int32_t) ( ( (entry) & 0xff000000 ) | (offset) )
#define MBCS_ENTRY_TRANSITION_ADD_OFFSET( entry, offset ) \
		(int32_t) ( (entry) + (offset) )

#define MBCS_ENTRY_FINAL( state, action, value ) \
		(int32_t) ( 0x80000000 | ( (int32_t) (state) << 24L ) | ( (action) << 20L ) | (value) )
#define MBCS_ENTRY_SET_FINAL( entry ) \
		(int32_t) ( (entry) | 0x80000000 )
#define MBCS_ENTRY_FINAL_SET_ACTION( entry, action ) \
		(int32_t) ( ( (entry) & 0xff0fffff ) | ( (int32_t) (action) << 20L ) )
#define MBCS_ENTRY_FINAL_SET_VALUE( entry, value ) \
		(int32_t) ( ( (entry) & 0xfff00000 ) | (value) )
#define MBCS_ENTRY_FINAL_SET_ACTION_VALUE( entry, action, value ) \
		(int32_t) ( ( (entry) & 0xff000000 ) | ( (int32_t) (action) << 20L ) | (value) )

#define MBCS_ENTRY_SET_STATE( entry, state ) \
		(int32_t) ( ( (entry) & 0x80ffffff ) | ( (int32_t) (state) << 24L ) )

#define MBCS_ENTRY_STATE( entry ) ( ( (entry) >> 24 ) & 0x7f )

#define MBCS_ENTRY_IS_TRANSITION( entry ) ( (entry) >= 0 )
#define MBCS_ENTRY_IS_FINAL( entry ) ( (entry) < 0 )

#define MBCS_ENTRY_TRANSITION_STATE( entry ) ( (entry) >> 24 )
#define MBCS_ENTRY_TRANSITION_OFFSET( entry ) ( (entry) & 0xffffff )

#define MBCS_ENTRY_FINAL_STATE( entry ) ( ( (entry) >> 24 ) & 0x7f )
#define MBCS_ENTRY_FINAL_IS_VALID_DIRECT_16( entry ) ( (entry) < (int32_t) 0x80100000 )
#define MBCS_ENTRY_FINAL_ACTION( entry ) ( ( (entry) >> 20 ) & 0xf )
#define MBCS_ENTRY_FINAL_VALUE( entry ) ( (entry) & 0xfffff )
#define MBCS_ENTRY_FINAL_VALUE_16( entry ) (uint16_t) (entry)


#define MBCS_STAGE_2_FROM_U( table, c ) \
		( (const uint32_t*) (table) ) [ (table) [ (c) >> 10 ] + ( ( (c) >> 4 ) & 0x3f ) ]
#define MBCS_FROM_U_IS_ROUNDTRIP( stage2_entry, c ) \
		( ( (stage2_entry) & ( (uint32_t) 1 << ( 16 + ( (c) & 0xf ) ) ) ) != 0 )

#define MBCS_VALUE_1_FROM_STAGE_2( bytes, stage2_entry, c) \
		( (uint8_t*) (bytes) ) [ 16 * (uint32_t) (uint16_t) (stage2_entry) + ( (c) &0xf ) ]

#define MBCS_VALUE_2_FROM_STAGE_2( bytes, stage2_entry, c) \
		( (uint16_t*) (bytes) ) [ 16 * (uint32_t) (uint16_t) (stage2_entry) + ( (c) &0xf ) ]

#define MBCS_VALUE_4_FROM_STAGE_2( bytes, stage2_entry, c) \
		( (uint32_t*) (bytes) ) [ 16 * (uint32_t) (uint16_t) (stage2_entry) + ( (c) & 0xf ) ]

#define MBCS_POINTER_3_FROM_STAGE_2( bytes, stage2_entry, c ) \
		( (bytes) + ( 16 * (uint32_t) (uint16_t) (stage2_entry) + ( (c) & 0xf ) ) * 3 )

enum
{
    MBCS_OUTPUT_1 = 0,
    MBCS_OUTPUT_2,
    MBCS_OUTPUT_3,
    MBCS_OUTPUT_4,

    MBCS_OUTPUT_COUNT,
};


typedef struct
{
    uint32_t	offset;
    ucs4_t		code_point;
} _mbcs_to_u_fallback;


typedef struct __kuconv_mbcs_data
{
    /* to uni */
    uint8_t count_states;
    uint32_t count_to_u_fallbacks;

    const int32_t (*states) [ 256 ];
    const uint16_t* unicode_code_units;
    const _mbcs_to_u_fallback* to_u_fallbacks;

    /* from uni */
    const uint16_t *from_u_table;
    const uint8_t *from_u_bytes;
    uint32_t from_u_bytes_len;
	
    uint8_t type;
	uint8_t uni_flags;


} kuconv_mbcs_data;

typedef struct
{
	uint32_t type;

	uint32_t count_states;
	uint32_t count_to_u_fallbacks;
	uint32_t offset_to_u_code_units;

	uint32_t offset_from_u_table;
	uint32_t offset_from_u_bytes;
	uint32_t from_u_bytes_len;
}
_mbcs_header;



int mbcs_load( conv_info* cnv_info, const uint8_t* raw_data );

extern conv_info mbcs_data;


#endif /* __KUCONV_MBCS_H */
