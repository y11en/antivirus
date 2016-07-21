#include "compatutils.h"
#include <errno.h>

errno_t wcscat_s(wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource)
{
	if(!strDestination)
		return EINVAL;

	if(!strSource){
		strDestination[0] = 0;
		return EINVAL;
	}

	if(!numberOfElements)
		return EINVAL;
	
	if((wcslen(strSource) + wcslen(strDestination)) >= numberOfElements){
		strDestination[0] = 0;
		return ERANGE;
	}

	wcsncat(strDestination, strSource, numberOfElements);

	return 0;
}
