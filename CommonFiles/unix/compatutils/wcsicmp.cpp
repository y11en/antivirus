#include <wctype.h>
#include "compatutils.h"

int _wcsicmp ( const wchar_t *str1, const wchar_t *str2 )
{
	if ( str1 == str2 )
		return 0;
	while ( *str1 && *str2 ) {
		wint_t ch1 = towlower ( *str1++ ); 
		wint_t ch2 = towlower ( *str2++ );  
		if ( ch1 > ch2 )
			return 1;
		else if ( ch1 < ch2 )
			return -1;
	}
	if ( *str1 && !*str2 )
		return 1;
	else if ( !*str1 && *str2 ) 
		return -1;
	return 0;
		
}

