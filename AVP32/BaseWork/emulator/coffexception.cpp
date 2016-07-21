/*
 *  coffexception.cpp,v 1.2 2002/10/17 09:55:01 tim 
 */

#include "typedefs.h"


CoffException::CoffException (const char * fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
	vsnprintf(text, sizeof(text), fmt, ap);
    text[sizeof(text) - 1] = '\0';
	va_end(ap);
}


const char * CoffException::GetErr()
{
	return text;
}
