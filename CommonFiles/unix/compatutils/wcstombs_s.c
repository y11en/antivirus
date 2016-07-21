#define _GNU_SOURCE
#include "compatutils.h"
#include <errno.h>
#include <string.h>


errno_t wcstombs_s(size_t *pReturnValue, char *mbstr, size_t sizeInBytes, const wchar_t *wcstr, size_t count)
{
	if(!mbstr && (sizeInBytes > 0))
		return EINVAL;

	if(mbstr)
		mbstr[0] = 0;

	if(!wcstr)
		return EINVAL;

	if(pReturnValue)
		*pReturnValue = 0;

	size_t nwc = count;
	if(count == _TRUNCATE)
		nwc = wcslen(wcstr);

	mbstate_t state;
	memset(&state, 0, sizeof(state));
	size_t result = wcsnrtombs(mbstr, &wcstr, nwc, sizeInBytes, &state);

	if(result == (size_t)-1){
		if(mbstr)
			mbstr[0] = 0;
		return errno;
	}

	if(!mbstr)
		++result;
	else if(*wcstr){
		if(sizeInBytes == result){
			if(count == _TRUNCATE){
				errno = STRUNCATE;
				mbstr[result - 1] = 0;
			}
			else {
				errno = ERANGE;
				mbstr[0] = 0;
				result = 0;
			}
			
		}
		else
			mbstr[result++] = 0;

	}

	if(pReturnValue)
		*pReturnValue = result;

	return errno;
}
