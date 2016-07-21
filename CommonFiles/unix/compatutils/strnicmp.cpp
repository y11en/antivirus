#include <ctype.h>
#include "compatutils.h"

int _strnicmp ( const char *str1, const char *str2, size_t n )
{
	if ( str1 == str2 )
		return 0;
	while ( *str1 && *str2 && n ) {
		--n;
		int ch1 = tolower ( *str1++ ); 
		int ch2 = tolower ( *str2++ );  
		if ( ch1 > ch2 )
			return 1;
		else if ( ch1 < ch2 )
			return -1;
	}
	if ( !n )
		return 0;
	if ( *str1 && !*str2 )
		return 1;
	else if ( !*str1 && *str2 ) 
		return -1;
	return 0;
}

