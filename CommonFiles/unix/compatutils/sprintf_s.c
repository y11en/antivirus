#ifndef _WIN32

#include "compatutils.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/* http://msdn2.microsoft.com/en-us/library/ce3zzk1k(VS.80).aspx */

int sprintf_s( char *buffer, size_t sizeOfBuffer, const char *format, ... )
{
	int vsnprintfResult;
	int resultLen;
	
	errno = 0;

	/* Check for invalid parameters */
	if( ( buffer == NULL ) || ( format == NULL ) )
	{
		errno = EINVAL;
		return -1;
	}

	
	va_list ap;
	va_start (ap, format);
	vsnprintfResult = vsnprintf( buffer, sizeOfBuffer, format, ap );
	va_end (ap);
	
	resultLen = strlen( buffer );
	
	if( vsnprintfResult != resultLen )	/* some kind of overrun prevented, vsnprintf returned more then could fit the buffer */
	{
		buffer[0] = 0;
		errno = ERANGE;
		resultLen = -1;
	}
	
	return resultLen;
} /* sprintf_s */


#endif /* _WIN32 */
