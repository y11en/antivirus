

#include "utils.h"

#include <stddef.h>

const char* skip_ws( const char* s ) 
{
    while( *s == ' ' || *s == '\t' )
	{
        ++s;
    }

    return s;
}

char* prepare_line( char* line, char** pend )
{
	char* end;
	char c;

	for( end = line; ( c = *end ) != '\0'; ++end )
	{
		if( c == '#' || c == '\r' || c == '\n' )
		{
			break;
		}
	}

	while( end > line && ( ( c = *( end - 1 ) ) == ' ' || c == '\t' ) ) 
	{
		--end;
	}
	*end = '\0';

	if( pend != NULL )
	{
		*pend = end;
	}

	return (char*) skip_ws( line );
}

