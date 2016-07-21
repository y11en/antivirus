#ifndef _WIN32

#include "compatutils.h"
#include <errno.h>
#include <string.h>

errno_t strncpy_s( char *strDestination, size_t numberOfElements, const char *strSource, size_t count)
{

	char* pos;
	size_t num;
	/* Check for invalid destination */
	if(strDestination == NULL)
	{
		return EINVAL;
	}

	/* Check for zero elements */
	if( numberOfElements == 0 )
	{
		return EINVAL;
	}

	if(count == 0)
	{
		strDestination[0] = 0;
		return 0;
	}

	/* Check for invalid source */
	if(strSource == NULL)
	{
		strDestination[0] = 0;
		return EINVAL;
	}

	pos = strDestination;
	num = numberOfElements;
	if(count == _TRUNCATE)
		while (((*pos++ = *strSource++) != 0) && (--num > 0));
	else{
		while (((*pos++ = *strSource++) != 0) && (--num > 0) && (--count > 0));
		if(count == 0)
			*pos = 0;
	}
	
	if(num == 0){
		if(count == _TRUNCATE){
			strDestination[numberOfElements - 1] = 0;
			return STRUNCATE;
		}
		strDestination[0] = 0;
		return ERANGE;
	}
	return 0;
} /* strcpy_s */

#endif /* _WIN32 */
