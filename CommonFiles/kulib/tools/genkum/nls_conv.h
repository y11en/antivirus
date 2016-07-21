
#ifndef __NLS_CONV_H
#define __NLS_CONV_H

#include "converter.h"

class nls_converter_info :
	public converter_info
{
public:
	nls_converter_info();
	virtual ~nls_converter_info();

	void get_encodings( encodings_map& encodings, int argc, char* argv[] );
	int is_ignorable_encoding( const std::string& cnv_id );

	virtual converter* create_converter( const std::string& cnv_id );
	virtual void destroy_converter( converter* cnv );

public:
	static converter_info* create();

public:

};

class nls_converter :
	public converter
{
public:
	nls_converter( const std::string& cnv_id, nls_converter_info* cnv_info );
	~nls_converter();

	int from_ucs( ucs_t c, char* dest, int dest_len );
	int to_ucs( const char* src, int src_len, ucs_t* dst, int dst_len );

	void get_codepage_info( codepage_info* cp_info );

	const char* get_name();
	const char* get_interface_name();

public:
	int m_codepage;
};

#endif // __NLS_CONV_H
