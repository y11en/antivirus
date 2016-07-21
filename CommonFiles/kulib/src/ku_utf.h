

#ifndef __KU_UTF_H 
#define __KU_UTF_H 

#define KU_IS_SURROGATE( u )    ( ( ( u ) & 0xfffff800 ) == 0xd800 )
#define KU_IS_SINGLE( u )       !KU_IS_SURROGATE( u )
#define KU_IS_LEAD( u )			( ( ( u ) & 0xfffffc00 ) == 0xd800 )
#define KU_IS_TRAIL( u )		( ( ( u ) & 0xfffffc00 ) == 0xdc00 )
#define KU_IS_SURROGATE_LEAD( u )  ( ( ( u ) & 0x400 ) == 0 ) 

#define KU_SURROGATE_OFFSET		( ( 0xd800 << 10UL ) + 0xdc00 - 0x10000 )

#define KU_GET_SUPPLEMENTARY( lead, trail ) \
    ( ( (ucs4_t) ( lead ) << 10 ) + (ucs4_t) ( trail ) - KU_SURROGATE_OFFSET )

#define KU_LEAD( sup ) (uint16_t) ( ( ( sup ) >> 10 ) + 0xd7c0 )

#define KU_TRAIL( sup ) (uint16_t) ( ( ( sup ) & 0x3ff ) | 0xdc00 )


#endif /* __KU_UTF_H */

