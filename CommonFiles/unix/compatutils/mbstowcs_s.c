#define _GNU_SOURCE
#include "compatutils.h"
#include <errno.h>
#include <string.h>


errno_t mbstowcs_s(size_t *pReturnValue, wchar_t *wcstr, size_t sizeInWords, const char *mbstr, size_t count)
{
	if(!wcstr && (sizeInWords > 0))
		return EINVAL;
	
	if(wcstr)
		wcstr[0] = 0;

	if(!mbstr)
		return EINVAL;


	if(pReturnValue)
		*pReturnValue = 0;

	size_t nmb = count;
	if(count == _TRUNCATE)
		nmb = strlen(mbstr);

	mbstate_t state;
	memset(&state, 0, sizeof(state));
	size_t result = mbsnrtowcs(wcstr, &mbstr, nmb, sizeInWords, &state);

	if(result == (size_t)-1){
		if(wcstr)
			wcstr[0] = 0;
		return errno;
	}

	if(!wcstr)
		++result;
	else if(*mbstr){
		if(sizeInWords == result){
			if(count == _TRUNCATE){
				errno = STRUNCATE;
				wcstr[result - 1] = 0;
			}
			else{
				errno = ERANGE;
				wcstr[0] = 0;
				result = 0;
			}
		}
		else
			wcstr[result++] = 0;
	}

	if(pReturnValue)
		*pReturnValue = result;


	return errno;
}
