#include <stdio.h>
#include <wchar.h>
#include "compatutils.h"

wchar_t* _itow ( int value, wchar_t* str, int radix )
{
	const wchar_t hex_table [] = { L"0123456789ABCDEF" };
	const wchar_t dec_table [] = { L"0123456789" };
	if ( !str )
		return 0;
	wchar_t* dest = str;
	const wchar_t* table = 0;
	if ( radix == 16 ) {
		*dest++ = L'0';
		*dest++ = L'x';
		table = hex_table;
	} else if ( radix == 10  ) {
		if ( value < 0 )
			*dest++ = L'-';
		table = dec_table;
		value = -value;
	}
	unsigned int tmp = value;
	int i = 1;
	while ( tmp ) {
		tmp /= radix;
		if ( tmp )
			i *= radix;
	}
	
	tmp = value;
	do {
		*dest++ = table [ tmp / i ];
		tmp %= i;
		i /= radix;
	}
	while ( i );
	*dest = 0;		  
	return str;
}

