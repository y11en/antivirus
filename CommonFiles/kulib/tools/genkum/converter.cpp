

#include "mlang_conv.h"
#include "nls_conv.h"

struct converter_descr
{
	const char* name;
	converter_info* (*create_proc)();
};

converter_descr cnv_list[] =
{
	"nls", nls_converter_info::create,
	"mlang", mlang_converter_info::create
};

converter_info* converter_info::get_converter_info( const char* cnv_name )
{
	if( !cnv_name )
	{
		return cnv_list[ 0 ].create_proc();
	}

	for( int i = 0; i < sizeof(cnv_list)/sizeof(cnv_list[0]); ++i )
	{
		if( !strcmp( cnv_name, cnv_list[ i ].name ) )
		{
			return cnv_list[ i ].create_proc();
		}
	}

	return NULL;
//	return new mlang_converter_info;
//	return new nls_converter_info;
}

void converter_info::release_converter_info( converter_info* cnv_info )
{
//	delete static_cast<mlang_converter_info*>( cnv_info );
	delete cnv_info;
}

converter_info::~converter_info()
{
}
