#ifndef _WIN32

#include "compatutils.h"
#include <errno.h>
#include <string.h>

errno_t strcpy_s( char *strDestination, size_t numberOfElements, const char *strSource )
{

	/* Check for invalid destination */
	if( strDestination == NULL )
	{
		return EINVAL;
	}

	/* Check for invalid source */
	if( strSource == NULL )
	{
		strDestination[0] = 0;
		return EINVAL;
	}

	/* Check for zero elements */
	if( numberOfElements == 0 )
	{
		return EINVAL;
	}
	
	/* Check for overrun */
	if(strlen( strSource ) >= numberOfElements )
	{
		strDestination[0] = 0;
		return ERANGE;
	}

	strncpy( strDestination, strSource, numberOfElements );

	return 0;
} /* strcpy_s */

#endif /* _WIN32 */
