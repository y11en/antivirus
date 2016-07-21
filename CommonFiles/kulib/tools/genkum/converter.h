
#ifndef __CONVERTER_H
#define __CONVERTER_H

#pragma warning( disable : 4786 )

#include <kulib/ku_defs.h>

#include <map>
#include <string>


#define KU_IS_SURROGATE( u )	( ( ( u ) & 0xfffff800 ) == 0xd800 )
#define KU_IS_LEAD( u )			( ( ( u ) & 0xfffffc00 ) == 0xd800 )
#define KU_IS_TRAIL( u )		( ( ( u ) & 0xfffffc00 ) == 0xdc00 )

#define KU_SURROGATE_OFFSET		( ( 0xd800 << 10UL ) + 0xdc00 - 0x10000 )

#define KU_GET_SUPPLEMENTARY( lead, trail ) \
    ( ( (ucs4_t) ( lead ) << 10 ) + (ucs4_t) ( trail ) - KU_SURROGATE_OFFSET )



#define MAX_UNICODE_VALUE	0x10ffff
#define MAX_MB_LEN			8

enum byte_type
{
	BYTE_UNUSED = 0,
	BYTE_END,
	BYTE_CONT,
};

typedef uint8_t byte_info[ MAX_MB_LEN ][ 0x100 ];


struct encoding_info
{
	char name[ 100 ];
	char description[ 100 ];	
};

struct codepage_info
{
	char defchar[ 16 ];
	int min_char_len;
	int max_char_len;
};

/*
enum mapping_type
{
	ROUNDTRIP  = 0,
	FALLBACK,
	REVERSE_FALLBACK
};
*/

typedef std::map< std::string, encoding_info* > encodings_map;

class converter
{
public:
	virtual int from_ucs( ucs_t c, char* dest, int dest_len ) = 0;
	virtual int to_ucs( const char* src, int len, ucs_t* dst, int dst_len ) = 0;

	virtual void get_codepage_info( codepage_info* cp_info ) = 0;

	virtual const char* get_name() = 0;
	virtual const char* get_interface_name() = 0;
};

class converter_info
{
public:
	virtual ~converter_info();

	virtual void get_encodings( encodings_map& encodings, int argc, char* argv[] ) = 0;
	virtual int is_ignorable_encoding( const std::string& cnv_id ) = 0;

	virtual converter* create_converter( const std::string& cnv_id ) = 0;
	virtual void destroy_converter( converter* cnv ) = 0;

public:
	static converter_info* get_converter_info( const char* cnv_name = 0 );
	static void release_converter_info( converter_info* cnv );

};


#endif // __CONVERTER_H
