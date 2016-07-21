#ifndef _WIN32

#include "compatutils.h"
#include <errno.h>
#include <string.h>
errno_t memcpy_s( void *dest, size_t numberOfElements, const void *src, size_t count )
{

	/* Check for invalid destination */
	if( dest == NULL )
	{
		return EINVAL;
	}

	/* Check for invalid source */
	if( src == NULL )
	{
		((char *)dest)[0] = 0;
		return EINVAL;
	}

	/* Check for zerosize destination buffer */
	if( numberOfElements == 0 )
	{
		return ERANGE;
	}
	
	/* Check for zero size source */
	if( count == 0 )
	{
		return 0;
	}
	
	/* Check for overrun */
	if(count > numberOfElements )
	{
		memset(dest, 0, numberOfElements);
		return ERANGE;
	}

	memcpy( dest, src, numberOfElements );

	return 0;
} /* memcpy_s */

#endif /* _WIN32 */

