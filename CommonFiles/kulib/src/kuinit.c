


#include "kuconv_int.h"

#include "kuinit_int.h"

int ku_init( void )
{
	int ret;

	if( ( ret = _kudata_init() ) != KU_OK )
		return ret;

	if( ( ret = _kuconv_case_init() ) != KU_OK )
		return ret;

	if( ( ret = _kuconv_alias_init() ) != KU_OK )
		return ret;

	if( ( ret = _kuconv_init() ) != KU_OK )
		return ret;


	return KU_OK;
}

int ku_term( void )
{
	_kuconv_term();
	_kuconv_alias_term();

	_kuconv_case_term();

	_kudata_term();

	return KU_OK;
}
