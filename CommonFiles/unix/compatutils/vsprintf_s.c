#include "compatutils.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>

int vsprintf_s(char *buffer, size_t numberOfElements, const char *format, va_list argptr) 
{
	int result;
	int len;
	
	errno = 0;

	if(!buffer || !format){
		errno = EINVAL;
		return -1;
	}

	
	result = vsnprintf(buffer, numberOfElements, format, argptr);
	
	len = strlen(buffer);
	
	if(result != len){	/* some kind of overrun prevented, vsnprintf returned more then could fit the buffer */
		buffer[0] = 0;
		errno = ERANGE;
		len = -1;
	}
	
	return len;
}
