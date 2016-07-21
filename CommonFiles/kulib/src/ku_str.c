


#include "ku_str.h"

int32_t ku_ucslen( const ucs_t* s ) 
{
	const ucs_t* t = s;

	while( *t != 0 ) 
	{
		++t;
	}

	return t - s;
}
