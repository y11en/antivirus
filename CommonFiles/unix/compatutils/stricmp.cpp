#include <ctype.h>
#include "compatutils.h"

int _stricmp ( const char *str1, const char *str2 )
{
	if ( str1 == str2 )
		return 0;
	while ( *str1 && *str2 ) {
		int ch1 = tolower ( *str1++ ); 
		int ch2 = tolower ( *str2++ );  
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

