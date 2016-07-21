#ifndef _WIN32

#include "compatutils.h"
#include <errno.h>
#include <string.h>

errno_t strcat_s( char *strDestination, size_t numberOfElements, const char *strSource )
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
	if( (strlen( strSource ) + strlen(strDestination) ) >= numberOfElements )
	{
		strDestination[0] = 0;
		return ERANGE;
	}

	strncat( strDestination, strSource, numberOfElements );

	return 0;
} /* strcat_s */


#endif /* _WIN32 */
