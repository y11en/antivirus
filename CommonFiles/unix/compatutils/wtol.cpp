#include <stdio.h>
#include <wchar.h>
#include "compatutils.h"

// only hex or decimal values are supported!

long _wtol ( const wchar_t* str  )
{
	if ( !str )
		return 0;
	while ( *str && *str == L' ' )
		++str;
	int len = wcslen ( str );
	while ( str [ len ] == L' ' )
		--len;
	if ( !len )
		return 0;
	unsigned char radix = 10;
	bool negative = false;
	long value = 0;
	if ( ( len > 2 ) && 
	     ( str [ 0 ] == L'0' ) && 
	     ( ( str [ 1 ] == L'X' ) || 
	       ( str [ 1 ] == L'x' ) ) ) {
		radix = 16;
		str += 2;
		len -= 2;
	}
	else if ( str [0] == L'-' ) {
		negative = true;
		++str;
		--len;
	}
	else if ( str [0] == L'+' ) {
		++str;
		--len;
	}
	while ( ( radix == 10 ) && *str && *str == L' ' ) {
		++str;
		--len;
	}
	if ( !len )
		return 0;
	const wchar_t* pos = str + len;
	int i = 1;
	do {
		--pos;
		if ( ( *pos >= L'0') && ( *pos <= L'9' ) )
			value += ( *pos - L'0' ) * i;
		else if ( ( radix == 16 ) && ( *pos >= L'A' ) && ( *pos <= L'F' ) )
			value += ( *pos - L'A' + 10 ) * i;			
		else if ( ( radix == 16 ) && ( *pos >= L'a' ) && ( *pos <= L'f' ) )
			value += ( *pos - L'a' + 10 ) * i;			
		else
			return 0;
		i *= radix;
	}
	while ( pos != str );
	if ( ( radix == 10 ) && negative )
		value = -value;
	return value;       
}
