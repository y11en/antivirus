#ifndef _WIN32

#include "compatutils.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* http://msdn2.microsoft.com/en-us/library/f30dzcf6(VS.80).aspx */

int _snprintf_s( char *buffer, size_t sizeOfBuffer, size_t count, const char *format, ... )
{
	int vsnprintfResult;
	int resultLen;
	int bytesToPrint;
	
	errno = 0;

	/* Check for invalid parameters */
	if( ( buffer == NULL ) || ( format == NULL ) )
	{
		errno = EINVAL;
		return -1;
	}

	/* Check for empty count */
	if( count == 0 )
	{
		return 0;
	}
	
	/* Determine if caller passed _TRUNCATE value ((size_t)-1) */
	bytesToPrint = (count == _TRUNCATE) ? sizeOfBuffer : count;

	/* If enought space, increment limit to fit trailing '\0' symbol */
	if (bytesToPrint < sizeOfBuffer )
	{
		bytesToPrint ++;
	}
	
	va_list ap;
	va_start (ap, format);
	vsnprintfResult = vsnprintf( buffer, bytesToPrint, format, ap );
	va_end (ap);
	
	resultLen = strlen( buffer );
	
	if( vsnprintfResult != resultLen )	/* some kind of overrun prevented, vsnprintf returned more then could fit the buffer */
	{
		if( sizeOfBuffer == count )
		{
			buffer[0] = 0;
			errno = ERANGE;
		}
		resultLen = -1;
	}
	
	return resultLen;
} /* _snprintf_s */



#endif /* _WIN32 */
